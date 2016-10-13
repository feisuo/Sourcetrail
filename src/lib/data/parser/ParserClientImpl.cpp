#include "data/parser/ParserClientImpl.h"

#include "data/parser/ParseLocation.h"
#include "data/graph/Node.h"
#include "data/graph/Edge.h"
#include "utility/logging/logging.h"
#include "utility/utility.h"
#include "data/location/TokenLocation.h"

ParserClientImpl::ParserClientImpl()
{
}

ParserClientImpl::~ParserClientImpl()
{
}

void ParserClientImpl::setStorage(std::shared_ptr<IntermediateStorage> storage)
{
	m_storage = storage;
}

void ParserClientImpl::resetStorage()
{
	m_storage.reset();
}

void ParserClientImpl::startParsingFile()
{
}

void ParserClientImpl::finishParsingFile()
{
}


Id ParserClientImpl::recordSymbol(
	const NameHierarchy& symbolName, SymbolKind symbolType,
	AccessKind access, bool isImplicit
)
{
	Id nodeId = addNodeHierarchy(symbolKindToNodeType(symbolType), symbolName, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addAccess(nodeId, access);
	return nodeId;
}

Id ParserClientImpl::recordSymbol(
	const NameHierarchy& symbolName, SymbolKind symbolType,
	const ParseLocation& location,
	AccessKind access, bool isImplicit
)
{
	Id nodeId = recordSymbol(symbolName, symbolType, access, isImplicit);
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	return nodeId;
}

Id ParserClientImpl::recordSymbol(
	const NameHierarchy& symbolName, SymbolKind symbolType,
	const ParseLocation& location, const ParseLocation& scopeLocation,
	AccessKind access, bool isImplicit
)
{
	Id nodeId = recordSymbol(symbolName, symbolType, location, access, isImplicit);
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
	return nodeId;
}

void ParserClientImpl::recordReference(
	ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
	const ParseLocation& location)
{
	Id contextNodeId = addNodeHierarchy(Node::NODE_UNDEFINED, contextName, DEFINITION_NONE);
	Id referencedNodeId = addNodeHierarchy(Node::NODE_UNDEFINED, referencedName, DEFINITION_NONE);
	Id edgeId = addEdge(referenceKindToEdgeType(referenceKind), contextNodeId, referencedNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}



void ParserClientImpl::onError(const ParseLocation& location, const std::string& message, bool fatal, bool indexed)
{
	log(std::string(fatal ? "FATAL: " : "ERROR: "), message, location);

	if (!location.isValid())
	{
		return;
	}

	addError(message, fatal, indexed, location);
}

void ParserClientImpl::onTypedefParsed(
	const ParseLocation& location, const NameHierarchy& typedefName, AccessKind access, bool isImplicit)
{
	log("typedef", typedefName.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_TYPEDEF, typedefName, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addAccess(nodeId, access);
}

void ParserClientImpl::onClassParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessKind access,
	const ParseLocation& scopeLocation, bool isImplicit)
{
	log("class", nameHierarchy.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(
		Node::NODE_CLASS,
		nameHierarchy,
		(isImplicit ? DEFINITION_IMPLICIT : (scopeLocation.isValid() ? DEFINITION_EXPLICIT : DEFINITION_NONE))
	);
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
	addAccess(nodeId, access);
}

void ParserClientImpl::onStructParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessKind access,
	const ParseLocation& scopeLocation, bool isImplicit)
{
	log("struct", nameHierarchy.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(
		Node::NODE_STRUCT,
		nameHierarchy,
		(isImplicit ? DEFINITION_IMPLICIT : (scopeLocation.isValid() ? DEFINITION_EXPLICIT : DEFINITION_NONE))
	);
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
	addAccess(nodeId, access);
}

void ParserClientImpl::onGlobalVariableParsed(const ParseLocation& location, const NameHierarchy& variable, bool isImplicit)
{
	log("global", variable.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, variable, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onFieldParsed(const ParseLocation& location, const NameHierarchy& field, AccessKind access, bool isImplicit)
{
	log("field", field.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_FIELD, field, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addAccess(nodeId, access);
}

void ParserClientImpl::onFunctionParsed(
	const ParseLocation& location, const NameHierarchy& function, const ParseLocation& scopeLocation, bool isImplicit)
{
	log("function", function.getQualifiedNameWithSignature(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_FUNCTION, function, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
}

void ParserClientImpl::onMethodParsed(
	const ParseLocation& location, const NameHierarchy& method, AccessKind access, AbstractionType abstraction, // todo: remove abstractio... better: remove this method!
	const ParseLocation& scopeLocation, bool isImplicit)
{
	log("method", method.getQualifiedNameWithSignature(), location);

	Id nodeId = addNodeHierarchy(
		Node::NODE_METHOD,
		method,
		(isImplicit ? DEFINITION_IMPLICIT : ((location.isValid() && scopeLocation.isValid()) ? (DEFINITION_EXPLICIT) : DEFINITION_NONE))
	);
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
	addAccess(nodeId, access);
}

void ParserClientImpl::onNamespaceParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, const ParseLocation& scopeLocation, bool isImplicit)
{
	log("namespace", nameHierarchy.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_NAMESPACE, nameHierarchy, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
}

void ParserClientImpl::onEnumParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessKind access,
	const ParseLocation& scopeLocation, bool isImplicit)
{
	log("enum", nameHierarchy.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_ENUM, nameHierarchy, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
	addAccess(nodeId, access);
}

void ParserClientImpl::onEnumConstantParsed(const ParseLocation& location, const NameHierarchy& nameHierarchy, bool isImplicit)
{
	log("enum constant", nameHierarchy.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_ENUM_CONSTANT, nameHierarchy, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onTemplateParameterTypeParsed(
	const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy, bool isImplicit)
{
	log("template parameter type", templateParameterTypeNameHierarchy.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_TEMPLATE_PARAMETER_TYPE, templateParameterTypeNameHierarchy, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addAccess(nodeId, ACCESS_TEMPLATE_PARAMETER);
}

void ParserClientImpl::onLocalSymbolParsed(const std::string& name, const ParseLocation& location)
{
	log(
		"local symbol",
		name,
		location
		);

	Id localSymbolId = addLocalSymbol(name);
	addSourceLocation(localSymbolId, location, locationTypeToInt(LOCATION_LOCAL_SYMBOL));
}

void ParserClientImpl::onFileParsed(const FileInfo& fileInfo)
{
	log("file", fileInfo.path.str(), ParseLocation());

	addFile(fileInfo.path.fileName(), fileInfo.path.str(), fileInfo.lastWriteTime.toString());
}

void ParserClientImpl::onMacroDefineParsed(
	const ParseLocation& location, const NameHierarchy& macroNameHierarchy, const ParseLocation& scopeLocation)
{
	log("macro", macroNameHierarchy.getQualifiedName(), location);

	Id macroId = addNodeHierarchy(Node::NODE_MACRO, macroNameHierarchy, DEFINITION_EXPLICIT);
	addSourceLocation(macroId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(macroId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));

	Id fileNodeId = addFile(location.filePath.str());
	Id edgeId = addEdge(Edge::EDGE_MACRO_USAGE, fileNodeId, macroId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onCommentParsed(const ParseLocation& location)
{
	log("comment", "no name", location);

	addFile(location.filePath.str());
	addCommentLocation(location);
}

void ParserClientImpl::onInheritanceParsed(
	const ParseLocation& location, const NameHierarchy& childNameHierarchy,
	const NameHierarchy& parentNameHierarchy)
{
	log("inheritance", childNameHierarchy.getQualifiedName() + " : " + parentNameHierarchy.getQualifiedName(), location);

	Id childNodeId = addNodeHierarchy(Node::NODE_TYPE, childNameHierarchy, DEFINITION_NONE);
	Id parentNodeId = addNodeHierarchy(Node::NODE_TYPE, parentNameHierarchy, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_INHERITANCE, childNodeId, parentNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onMethodOverrideParsed(
	const ParseLocation& location, const NameHierarchy& overridden, const NameHierarchy& overrider)
{
	log("override", overridden.getQualifiedNameWithSignature() + " -> " + overrider.getQualifiedNameWithSignature(), location);

	Id overriddenNodeId = addNodeHierarchy(Node::NODE_FUNCTION, overridden, DEFINITION_NONE);
	Id overriderNodeId = addNodeHierarchy(Node::NODE_FUNCTION, overrider, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_OVERRIDE, overriderNodeId, overriddenNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onCallParsed(const ParseLocation& location, const NameHierarchy& caller, const NameHierarchy& callee)
{
	log("call", caller.getQualifiedNameWithSignature() + " -> " + callee.getQualifiedNameWithSignature(), location);

	Id callerNodeId = addNodeHierarchy(Node::NODE_FUNCTION, caller, DEFINITION_NONE);
	Id calleeNodeId = addNodeHierarchy(Node::NODE_FUNCTION, callee, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_CALL, callerNodeId, calleeNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onUsageParsed(
	const ParseLocation& location, const NameHierarchy& userName, SymbolKind usedType, const NameHierarchy& usedName)
{
	log("usage", userName.getQualifiedNameWithSignature() + " -> " + usedName.getQualifiedName(), location);

	Id userNodeId = addNodeHierarchy(Node::NODE_UNDEFINED, userName, DEFINITION_NONE);
	Id usedNodeId = addNodeHierarchy(symbolKindToNodeType(usedType), usedName, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_USAGE, userNodeId, usedNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onTypeUsageParsed(const ParseLocation& location, const NameHierarchy& user, const NameHierarchy& used)
{
	log("type usage", user.getQualifiedNameWithSignature() + " -> " + used.getQualifiedName(), location);

	if (!location.isValid())
	{
		return;
	}

	Id functionNodeId = addNodeHierarchy(Node::NODE_UNDEFINED, user, DEFINITION_NONE);
	Id typeNodeId = addNodeHierarchy(Node::NODE_TYPE, used, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_TYPE_USAGE, functionNodeId, typeNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onTemplateArgumentTypeParsed(
	const ParseLocation& location, const NameHierarchy& argumentTypeNameHierarchy,
	const NameHierarchy& templateNameHierarchy)
{
	log(
		"template argument type",
		argumentTypeNameHierarchy.getQualifiedName() + " -> " + templateNameHierarchy.getQualifiedName(),
		location
	);

	Id argumentNodeId = addNodeHierarchy(Node::NODE_TYPE, argumentTypeNameHierarchy, DEFINITION_NONE);
	Id templateNodeId = addNodeHierarchy(Node::NODE_UNDEFINED, templateNameHierarchy, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_TEMPLATE_ARGUMENT, templateNodeId, argumentNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onTemplateDefaultArgumentTypeParsed(
	const ParseLocation& location, const NameHierarchy& defaultArgumentTypeNameHierarchy,
	const NameHierarchy& templateParameterNameHierarchy)
{
	log(
		"template default argument",
		defaultArgumentTypeNameHierarchy.getQualifiedNameWithSignature() + " -> " + templateParameterNameHierarchy.getQualifiedName(),
		location
	);

	Id defaultArgumentNodeId = addNodeHierarchy(Node::NODE_TYPE, defaultArgumentTypeNameHierarchy, DEFINITION_NONE);
	Id parameterNodeId = addNodeHierarchy(Node::NODE_TYPE, templateParameterNameHierarchy, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_TEMPLATE_DEFAULT_ARGUMENT, parameterNodeId, defaultArgumentNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onTemplateSpecializationParsed(
	const ParseLocation& location, const NameHierarchy& specializedNameHierarchy,
	const NameHierarchy& specializedFromNameHierarchy)
{
	log(
		"template record specialization",
		specializedNameHierarchy.getQualifiedName() + " -> " + specializedFromNameHierarchy.getQualifiedName(),
		location
	);

	Id specializedId = addNodeHierarchy(Node::NODE_TYPE, specializedNameHierarchy, DEFINITION_NONE);
	Id recordNodeId = addNodeHierarchy(Node::NODE_TYPE, specializedFromNameHierarchy, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_TEMPLATE_SPECIALIZATION_OF, specializedId, recordNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onTemplateMemberFunctionSpecializationParsed(
	const ParseLocation& location, const NameHierarchy& instantiatedFunction, const NameHierarchy& specializedFunction)
{
	log(
		"template member function specialization",
		instantiatedFunction.getQualifiedNameWithSignature() + " -> " + specializedFunction.getQualifiedNameWithSignature(),
		location
	);

	Id instantiatedFunctionNodeId = addNodeHierarchy(Node::NODE_FUNCTION, instantiatedFunction, DEFINITION_NONE);
	Id specializedFunctionNodeId = addNodeHierarchy(Node::NODE_FUNCTION, specializedFunction, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_TEMPLATE_MEMBER_SPECIALIZATION_OF, instantiatedFunctionNodeId, specializedFunctionNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onFileIncludeParsed(const ParseLocation& location, const FileInfo& fileInfo, const FileInfo& includedFileInfo)
{
	log("include", includedFileInfo.path.str(), location);

	Id fileNodeId = addFile(fileInfo.path.fileName(), fileInfo.path.str(), fileInfo.lastWriteTime.toString());
	Id includedFileNodeId = addFile(includedFileInfo.path.fileName(), includedFileInfo.path.str(), includedFileInfo.lastWriteTime.toString());
	Id edgeId = addEdge(Edge::EDGE_INCLUDE, fileNodeId, includedFileNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onMacroExpandParsed(const ParseLocation &location, const NameHierarchy& macroNameHierarchy)
{
	log("macro use", macroNameHierarchy.getQualifiedName(), location);

	Id macroExpandId = addNodeHierarchy(Node::NODE_MACRO, macroNameHierarchy, DEFINITION_NONE);
	Id fileNodeId = addFile(location.filePath.str());
	Id edgeId = addEdge(Edge::EDGE_MACRO_USAGE, fileNodeId, macroExpandId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}

Node::NodeType ParserClientImpl::symbolKindToNodeType(SymbolKind symbolType) const
{
	switch (symbolType)
	{
	case SYMBOL_BUILTIN_TYPE:
		return Node::NODE_BUILTIN_TYPE;
	case SYMBOL_CLASS:
		return Node::NODE_CLASS;
	case SYMBOL_ENUM:
		return Node::NODE_ENUM;
	case SYMBOL_ENUM_CONSTANT:
		return Node::NODE_ENUM_CONSTANT;
	case SYMBOL_FIELD:
		return Node::NODE_FIELD;
	case SYMBOL_FUNCTION:
		return Node::NODE_FUNCTION;
	case SYMBOL_GLOBAL_VARIABLE:
		return Node::NODE_GLOBAL_VARIABLE;
	case SYMBOL_INTERFACE:
		return Node::NODE_INTERFACE;
	case SYMBOL_MACRO:
		return Node::NODE_MACRO;
	case SYMBOL_METHOD:
		return Node::NODE_METHOD;
	case SYMBOL_NAMESPACE:
		return Node::NODE_NAMESPACE;
	case SYMBOL_PACKAGE:
		return Node::NODE_PACKAGE;
	case SYMBOL_STRUCT:
		return Node::NODE_STRUCT;
	case SYMBOL_TEMPLATE_PARAMETER:
		return Node::NODE_TEMPLATE_PARAMETER_TYPE;
	case SYMBOL_TYPEDEF:
		return Node::NODE_TYPEDEF;
	case SYMBOL_TYPE_PARAMETER:
		return Node::NODE_TYPE_PARAMETER;
	case SYMBOL_UNION:
		return Node::NODE_TYPE;
	default:
		break;
	}
	return Node::NODE_UNDEFINED;
}

Edge::EdgeType ParserClientImpl::referenceKindToEdgeType(ReferenceKind referenceKind) const
{
	switch (referenceKind)
	{
	case REFERENCE_TYPE_USAGE:
		return Edge::EDGE_TYPE_USAGE;
	case REFERENCE_USAGE:
		return Edge::EDGE_USAGE;
	case REFERENCE_CALL:
		return Edge::EDGE_CALL;
	case REFERENCE_INHERITANCE:
		return Edge::EDGE_INHERITANCE;
	case REFERENCE_OVERRIDE:
		return Edge::EDGE_OVERRIDE;
	case REFERENCE_TEMPLATE_ARGUMENT:
		return Edge::EDGE_TEMPLATE_ARGUMENT;
	case REFERENCE_TYPE_ARGUMENT:
		return Edge::EDGE_TYPE_ARGUMENT;
	case REFERENCE_TEMPLATE_DEFAULT_ARGUMENT:
		return Edge::EDGE_TEMPLATE_DEFAULT_ARGUMENT;
	case REFERENCE_TEMPLATE_SPECIALIZATION_OF:
		return Edge::EDGE_TEMPLATE_SPECIALIZATION_OF;
	case REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION_OF:
		return Edge::EDGE_TEMPLATE_MEMBER_SPECIALIZATION_OF;
	case REFERENCE_INCLUDE:
		return Edge::EDGE_INCLUDE;
	case REFERENCE_IMPORT:
		return Edge::EDGE_IMPORT;
	case REFERENCE_MACRO_USAGE:
		return Edge::EDGE_MACRO_USAGE;
	default:
		break;
	}
	return Edge::EDGE_UNDEFINED;
}

void ParserClientImpl::addAccess(Id nodeId, AccessKind access)
{
	if (access != ACCESS_NONE)
	{
		addComponentAccess(nodeId, accessKindToInt(access));
	}
}

Id ParserClientImpl::addNodeHierarchy(Node::NodeType nodeType, NameHierarchy nameHierarchy, DefinitionType definitionType)
{
	if (nameHierarchy.size() == 0)
	{
		return 0;
	}

	Id parentNodeId = 0;
	NameHierarchy currentNameHierarchy;

	for (size_t i = 0; i < nameHierarchy.size(); i++)
	{
		currentNameHierarchy.push(nameHierarchy[i]);
		const bool currentIsLastElement = (i == nameHierarchy.size() - 1);
		Node::NodeType currentType = (currentIsLastElement ? nodeType : Node::NODE_UNDEFINED); // TODO: rename to unknown!
		DefinitionType currentDefinitionType = (currentIsLastElement ? definitionType : DEFINITION_NONE);

		Id nodeId = addNode(currentType, currentNameHierarchy, currentDefinitionType);

		// Todo: performance optimization: check if node exists. dont add edge if it existed before...
		if (parentNodeId != 0)
		{
			addEdge(Edge::EDGE_MEMBER, parentNodeId, nodeId);
		}

		parentNodeId = nodeId;
	}
	return parentNodeId;
}


Id ParserClientImpl::addFile(const std::string& name, const std::string& filePath, const std::string& modificationTime)
{
	if (!m_storage)
	{
		return 0;
	}

	return m_storage->addFile(name, filePath, modificationTime);
}

Id ParserClientImpl::addFile(const std::string& filePath)
{
	if (!m_storage)
	{
		return 0;
	}

	return m_storage->addFile("", filePath, "");
}

Id ParserClientImpl::addNode(Node::NodeType nodeType, NameHierarchy nameHierarchy, DefinitionType definitionType)
{
	if (!m_storage)
	{
		return 0;
	}

	return m_storage->addNode(Node::typeToInt(nodeType), NameHierarchy::serialize(nameHierarchy), definitionTypeToInt(definitionType));
}

Id ParserClientImpl::addEdge(int type, Id sourceId, Id targetId)
{
	if (!m_storage)
	{
		return 0;
	}

	if (!sourceId || !targetId)
	{
		return 0;
	}

	return m_storage->addEdge(type, sourceId, targetId);
}

Id ParserClientImpl::addLocalSymbol(const std::string& name)
{
	if (!m_storage)
	{
		return 0;
	}

	return m_storage->addLocalSymbol(name);
}

void ParserClientImpl::addSourceLocation(Id elementId, const ParseLocation& location, int type)
{
	if (!m_storage)
	{
		return;
	}

	if (!location.isValid())
	{
		return;
	}

	if (location.filePath.empty())
	{
		LOG_ERROR("no filename set!");
		return;
	}

	m_storage->addSourceLocation(
		elementId,
		addFile(location.filePath.str()),
		location.startLineNumber,
		location.startColumnNumber,
		location.endLineNumber,
		location.endColumnNumber,
		type
	);
}

void ParserClientImpl::addComponentAccess(Id nodeId , int type)
{
	if (!m_storage)
	{
		return;
	}

	m_storage->addComponentAccess(nodeId, type);
}

void ParserClientImpl::addCommentLocation(const ParseLocation& location)
{
	if (!m_storage)
	{
		return;
	}

	m_storage->addCommentLocation(
		addFile(location.filePath.str()),
		location.startLineNumber,
		location.startColumnNumber,
		location.endLineNumber,
		location.endColumnNumber
	);
}

void ParserClientImpl::addError(const std::string& message, bool fatal, bool indexed, const ParseLocation& location)
{
	if (!m_storage)
	{
		return;
	}

	m_storage->addError(message, fatal, indexed, location.filePath.str(), location.startLineNumber, location.startColumnNumber);
}

void ParserClientImpl::log(std::string type, std::string str, const ParseLocation& location) const
{
	LOG_INFO_STREAM_BARE(
		<< type << ": " << str << " <" << location.filePath.str() << " "
		<< location.startLineNumber << ":" << location.startColumnNumber << " "
		<< location.endLineNumber << ":" << location.endColumnNumber << ">"
	);
}
