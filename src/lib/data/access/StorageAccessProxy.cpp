#include "data/access/StorageAccessProxy.h"

#include "data/graph/Graph.h"
#include "data/location/TokenLocationCollection.h"
#include "data/location/TokenLocationFile.h"

#include "utility/logging/logging.h"
#include "utility/file/FileInfo.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/TimePoint.h"

StorageAccessProxy::StorageAccessProxy()
	: m_subject(nullptr)
{
}

StorageAccessProxy::~StorageAccessProxy()
{
}

bool StorageAccessProxy::hasSubject() const
{
	if (m_subject)
	{
		return true;
	}

	LOG_ERROR("StorageAccessProxy has no subject.");
	return false;
}

void StorageAccessProxy::setSubject(StorageAccess* subject)
{
	m_subject = subject;
}

Id StorageAccessProxy::getIdForNodeWithNameHierarchy(const NameHierarchy& nameHierarchy) const
{
	if (hasSubject())
	{
		return m_subject->getIdForNodeWithNameHierarchy(nameHierarchy);
	}

	return 0;
}

Id StorageAccessProxy::getIdForEdge(
	Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy
) const {
	if (hasSubject())
	{
		return m_subject->getIdForEdge(type, fromNameHierarchy, toNameHierarchy);
	}

	return 0;
}

Node::NodeType StorageAccessProxy::getNodeTypeForNodeWithId(Id id) const
{
	if (hasSubject())
	{
		return m_subject->getNodeTypeForNodeWithId(id);
	}
	return Node::NODE_UNDEFINED;
}

NameHierarchy StorageAccessProxy::getNameHierarchyForNodeWithId(Id id) const
{
	if (hasSubject())
	{
		return m_subject->getNameHierarchyForNodeWithId(id);
	}

	return NameHierarchy();
}

std::vector<SearchMatch> StorageAccessProxy::getAutocompletionMatches(const std::string& query) const
{
	if (hasSubject())
	{
		return m_subject->getAutocompletionMatches(query);
	}

	return std::vector<SearchMatch>();
}

std::shared_ptr<TokenLocationCollection> StorageAccessProxy::getFullTextSearchLocations(
		const std::string &searchTerm, bool caseSensitive) const
{
	if (hasSubject())
	{
		return m_subject->getFullTextSearchLocations(searchTerm, caseSensitive);
	}

	return std::make_shared<TokenLocationCollection>();
}

std::vector<SearchMatch> StorageAccessProxy::getSearchMatchesForTokenIds(const std::vector<Id>& tokenIds) const
{
	if (hasSubject())
	{
		return m_subject->getSearchMatchesForTokenIds(tokenIds);
	}

	return std::vector<SearchMatch>();
}

std::shared_ptr<Graph> StorageAccessProxy::getGraphForAll() const
{
	if (hasSubject())
	{
		return m_subject->getGraphForAll();
	}

	return std::make_shared<Graph>();
}

std::shared_ptr<Graph> StorageAccessProxy::getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const
{
	if (hasSubject())
	{
		return m_subject->getGraphForActiveTokenIds(tokenIds);
	}

	return std::make_shared<Graph>();
}

std::vector<Id> StorageAccessProxy::getActiveTokenIdsForId(Id tokenId, Id* delcarationId) const
{
	if (hasSubject())
	{
		return m_subject->getActiveTokenIdsForId(tokenId, delcarationId);
	}

	return std::vector<Id>();
}

std::vector<Id> StorageAccessProxy::getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const
{
	if (hasSubject())
	{
		return m_subject->getNodeIdsForLocationIds(locationIds);
	}

	return std::vector<Id>();
}

std::vector<Id> StorageAccessProxy::getLocalSymbolIdsForLocationIds(const std::vector<Id>& locationIds) const
{
	if (hasSubject())
	{
		return m_subject->getLocalSymbolIdsForLocationIds(locationIds);
	}

	return std::vector<Id>();
}

std::vector<Id> StorageAccessProxy::getTokenIdsForMatches(const std::vector<SearchMatch>& matches) const
{
	if (hasSubject())
	{
		return m_subject->getTokenIdsForMatches(matches);
	}

	return std::vector<Id>();
}

Id StorageAccessProxy::getTokenIdForFileNode(const FilePath& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getTokenIdForFileNode(filePath);
	}

	return 0;
}

std::shared_ptr<TokenLocationCollection> StorageAccessProxy::getTokenLocationsForTokenIds(
		const std::vector<Id>& tokenIds) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForTokenIds(tokenIds);
	}

	return std::make_shared<TokenLocationCollection>();
}

std::shared_ptr<TokenLocationCollection> StorageAccessProxy::getTokenLocationsForLocationIds(
		const std::vector<Id>& locationIds) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForLocationIds(locationIds);
	}

	return std::make_shared<TokenLocationCollection>();
}

std::shared_ptr<TokenLocationFile> StorageAccessProxy::getTokenLocationsForFile(const std::string& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForFile(filePath);
	}

	return std::make_shared<TokenLocationFile>("");
}

std::shared_ptr<TokenLocationFile> StorageAccessProxy::getTokenLocationsForLinesInFile(
	const std::string& filePath, uint firstLineNumber, uint lastLineNumber
) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForLinesInFile(filePath, firstLineNumber, lastLineNumber);
	}

	return std::make_shared<TokenLocationFile>("");
}

std::shared_ptr<TokenLocationFile> StorageAccessProxy::getCommentLocationsInFile(const FilePath& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getCommentLocationsInFile(filePath);
	}

	return std::make_shared<TokenLocationFile>("");
}

std::shared_ptr<TextAccess> StorageAccessProxy::getFileContent(const FilePath& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getFileContent(filePath);
	}

	return nullptr;
}

FileInfo StorageAccessProxy::getFileInfoForFilePath(const FilePath& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getFileInfoForFilePath(filePath);
	}

	return FileInfo();
}

std::vector<FileInfo> StorageAccessProxy::getFileInfosForFilePaths(const std::vector<FilePath>& filePaths) const
{
	if (hasSubject())
	{
		return m_subject->getFileInfosForFilePaths(filePaths);
	}

	return std::vector<FileInfo>();
}

StorageStats StorageAccessProxy::getStorageStats() const
{
	if (hasSubject())
	{
		return m_subject->getStorageStats();
	}

	return StorageStats();
}


ErrorCountInfo StorageAccessProxy::getErrorCount() const
{
	ErrorCountInfo info;

	std::vector<StorageError> storageErrors = getAllErrors();
	for (const StorageError& error : storageErrors)
	{
		info.total++;

		if (error.fatal)
		{
			info.fatal++;
		}
	}

	return info;
}

ErrorCountInfo StorageAccessProxy::getFilteredErrorCount() const
{
	ErrorCountInfo info;

	std::vector<StorageError> storageErrors = getAllErrors();
	for (const StorageError& error : storageErrors)
	{
		if (!m_errorFilter.filter(error))
		{
			continue;
		}

		info.total++;

		if (error.fatal)
		{
			info.fatal++;
		}
	}

	return info;
}

std::vector<StorageError> StorageAccessProxy::getAllErrors() const
{
	if (hasSubject())
	{
		return m_subject->getAllErrors();
	}

	return std::vector<StorageError>();
}

std::vector<StorageError> StorageAccessProxy::getFilteredErrors() const
{
	std::vector<StorageError> errors = getAllErrors();
	std::vector<StorageError> filteredErrors;

	for (const StorageError& error : errors)
	{
		if (m_errorFilter.filter(error))
		{
			filteredErrors.push_back(error);
		}
	}

	return filteredErrors;
}

std::shared_ptr<TokenLocationCollection> StorageAccessProxy::getErrorTokenLocations(std::vector<ErrorInfo>* errors) const
{
	if (hasSubject())
	{
		std::shared_ptr<TokenLocationCollection> collection = m_subject->getErrorTokenLocations(errors);
		std::vector<ErrorInfo> unfilteredErrors = *errors;
		errors->clear();

		for (const ErrorInfo& error : unfilteredErrors)
		{
			if (m_errorFilter.filter(error))
			{
				errors->push_back(error);
			}
			else
			{
				// Set first bit to 1 to avoid collisions
				Id locationId = ~(~size_t(0) >> 1) + error.id;
				collection->removeTokenLocation(collection->findTokenLocationById(locationId));
			}
		}

		return collection;
	}

	return std::make_shared<TokenLocationCollection>();
}

void StorageAccessProxy::handleMessage(MessageErrorFilterChanged* message)
{
	m_errorFilter = message->errorFilter;
	MessageShowErrors(getFilteredErrorCount()).dispatch();
}
