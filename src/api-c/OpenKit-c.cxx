/**
* Copyright 2018 Dynatrace LLC
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


#include "OpenKit.h"

#include "api-c/OpenKit-c.h"
#include "api-c/CustomLogger.h"
#include "api-c/CustomTrustManager.h"

#include "protocol/ssl/SSLBlindTrustManager.h"

#include <list>
#include <assert.h> 

extern "C" {

	// These macros are for increasing readability only
#define TRY try
#define CATCH_AND_LOG(handle)																			\
	catch (const std::exception& ex)																	\
	{																									\
		if (handle)																						\
		{																								\
			assert((handle)->logger != NULL);															\
			(handle)->logger->error("Exception occurred in %s #%d: %s", __FILE__, __LINE__, ex.what());	\
		}																								\
	}																									\
	catch (...)																							\
	{																									\
		if (handle)																						\
		{																								\
			assert((handle)->logger != NULL);															\
			(handle)->logger->error("Unknown exception occurred in %s #%d", __FILE__, __LINE__);		\
		}																								\
	}

	//--------------
	// TrustManager
	//--------------

	typedef struct TrustManagerHandle
	{
		std::shared_ptr<openkit::ISSLTrustManager> trustManager = nullptr;
	} TrustManagerHandle;

	TrustManagerHandle* createCustomTrustManager()
	{
		// Sanity
		TrustManagerHandle* handle = nullptr;
		try
		{
			auto trustManager = std::shared_ptr<openkit::ISSLTrustManager>(new apic::CustomTrustManager());
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new TrustManagerHandle();
			handle->trustManager = trustManager;
		}
		catch (...)
		{
			/* Ignore exception, as we don't have a logger yet */
		}

		return handle;
	}

	TrustManagerHandle* createBlindTrustManager()
	{
		// Sanity
		TrustManagerHandle* handle = nullptr;
		try
		{
			auto trustManager = std::shared_ptr<openkit::ISSLTrustManager>(new protocol::SSLBlindTrustManager());
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new TrustManagerHandle();
			handle->trustManager = trustManager;
		}
		catch (...)
		{
			/* Ignore exception, as we don't have a logger yet */
		}

		return handle;
	}

	void destroyTrustManager(TrustManagerHandle* trustManagerHandle)
	{
		// Sanity
		if (trustManagerHandle == nullptr)
		{
			return;
		}

		// release shared pointer
		trustManagerHandle->trustManager = nullptr;
		delete trustManagerHandle;
	}

	void disableSSLVerifiaction()
	{

	}

	//--------------
	//  Logger
	//--------------

	typedef struct LoggerHandle
	{
		std::shared_ptr<openkit::ILogger> logger = nullptr;
	} LoggerHandle;

	LoggerHandle* createLogger(levelEnabledFunc levelEnabledFunc, logFunc logFunc)
	{
		// Sanity
		if (levelEnabledFunc == nullptr || logFunc == nullptr)
		{
			return nullptr;
		}

		LoggerHandle* handle = nullptr;
		try
		{
			auto logger = std::shared_ptr<openkit::ILogger>(new apic::CustomLogger(levelEnabledFunc, logFunc));
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new LoggerHandle();
			handle->logger = logger;
		}
		catch (...)
		{
			/* Ignore exception, as we don't have a logger yet */
		}

		return handle;
	}

	void destroyLogger(LoggerHandle* loggerHandle)
	{
		// Sanity
		if (loggerHandle == nullptr)
		{
			return;
		}

		// release shared pointer
		loggerHandle->logger = nullptr;
		delete loggerHandle;
	}

	//--------------
	//  OpenKit
	//--------------

	typedef struct OpenKitHandle
	{
		std::shared_ptr<openkit::IOpenKit> sharedPointer = nullptr;
		std::shared_ptr<openkit::ILogger> logger = nullptr;
		std::shared_ptr<openkit::ISSLTrustManager> trustManager = nullptr;
	} OpenKitHandle;

	OpenKitHandle* createDynatraceOpenKitWithoutSSLVerification(const char* endpointURL, const char* applicationID, int64_t deviceID, LoggerHandle* loggerHandle,
		const char* applicationVersion, int32_t disableSSLVerification, const char* operatingSystem, const char* manufacturer,
		const char* modelID, int64_t beaconCacheMaxRecordAge, int64_t beaconCacheLowerMemoryBoundary, int64_t beaconCacheUpperMemoryBoundary)
	{
		TrustManagerHandle* trustManagerHandle = nullptr;
		if (disableSSLVerification != 0)
		{
			trustManagerHandle = createBlindTrustManager();
		}

		return createDynatraceOpenKit(endpointURL, applicationID, deviceID, loggerHandle, applicationVersion, trustManagerHandle, operatingSystem, manufacturer, modelID,
			beaconCacheMaxRecordAge, beaconCacheLowerMemoryBoundary, beaconCacheUpperMemoryBoundary);
	}

	OpenKitHandle* createDynatraceOpenKit(const char* endpointURL, const char* applicationID, int64_t deviceID, LoggerHandle* loggerHandle,
		const char* applicationVersion, TrustManagerHandle* trustManagerHandle, const char* operatingSystem, const char* manufacturer, 
		const char* modelID, int64_t beaconCacheMaxRecordAge, int64_t beaconCacheLowerMemoryBoundary, int64_t beaconCacheUpperMemoryBoundary)
	{
		OpenKitHandle* handle = nullptr;
		TRY
		{
			openkit::DynatraceOpenKitBuilder builder(endpointURL, applicationID, deviceID);
			if (loggerHandle)
			{
				// Instantiate the CustomLogger mapping the log statements to the FunctionPointers
				builder.withLogger(loggerHandle->logger);
			}

			if (applicationVersion != nullptr)
			{
				builder.withApplicationVersion(applicationVersion);
			}

			if (trustManagerHandle != nullptr)
			{
				builder.withTrustManager(trustManagerHandle->trustManager);
			}

			if (operatingSystem != nullptr)
			{
				builder.withOperatingSystem(operatingSystem);
			}

			if (manufacturer != nullptr)
			{
				builder.withManufacturer(manufacturer);
			}

			if (modelID != nullptr)
			{
				builder.withModelID(modelID);
			}

			if (beaconCacheMaxRecordAge >= 0)
			{
				builder.withBeaconCacheMaxRecordAge(beaconCacheMaxRecordAge);
			}

			if (beaconCacheLowerMemoryBoundary >= 0)
			{
				builder.withBeaconCacheLowerMemoryBoundary(beaconCacheLowerMemoryBoundary);
			}

			if (beaconCacheUpperMemoryBoundary >= 0)
			{
				builder.withBeaconCacheUpperMemoryBoundary(beaconCacheUpperMemoryBoundary);
			}

			std::shared_ptr<openkit::IOpenKit> openKit = builder.build();
		
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new OpenKitHandle();
			handle->sharedPointer = openKit;
			handle->logger = loggerHandle->logger;
			handle->trustManager = trustManagerHandle->trustManager;
		}
		CATCH_AND_LOG(loggerHandle)
		
		return handle;
	}

	OpenKitHandle* createAppMonOpenKitWithoutSSLVerification(const char* endpointURL, const char* applicationID, int64_t deviceID, LoggerHandle* loggerHandle,
		const char* applicationVersion, int32_t disableSSLVerification, const char* operatingSystem, const char* manufacturer,
		const char* modelID, int64_t beaconCacheMaxRecordAge, int64_t beaconCacheLowerMemoryBoundary, int64_t beaconCacheUpperMemoryBoundary)
	{
		TrustManagerHandle* trustManagerHandle = nullptr;
		if (disableSSLVerification != 0)
		{
			trustManagerHandle = createBlindTrustManager();
		}

		return createAppMonOpenKit(endpointURL, applicationID, deviceID, loggerHandle, applicationVersion, trustManagerHandle, operatingSystem, manufacturer, modelID,
			beaconCacheMaxRecordAge, beaconCacheLowerMemoryBoundary, beaconCacheUpperMemoryBoundary);
	}

	OpenKitHandle* createAppMonOpenKit(const char* endpointURL, const char* applicationID, int64_t deviceID, LoggerHandle* loggerHandle,
		const char* applicationVersion, TrustManagerHandle* trustManagerHandle, const char* operatingSystem, const char* manufacturer,
		const char* modelID, int64_t beaconCacheMaxRecordAge, int64_t beaconCacheLowerMemoryBoundary, int64_t beaconCacheUpperMemoryBoundary)
	{
		OpenKitHandle* handle = nullptr;
		TRY
		{
			openkit::AppMonOpenKitBuilder builder(endpointURL, applicationID, deviceID);
			if (loggerHandle)
			{
				// Instantiate the CustomLogger mapping the log statements to the FunctionPointers
				builder.withLogger(loggerHandle->logger);
			}

			if (applicationVersion != nullptr)
			{
				builder.withApplicationVersion(applicationVersion);
			}

			if (trustManagerHandle != nullptr)
			{
				builder.withTrustManager(trustManagerHandle->trustManager);
			}

			if (operatingSystem != nullptr)
			{
				builder.withOperatingSystem(operatingSystem);
			}

			if (manufacturer != nullptr)
			{
				builder.withManufacturer(manufacturer);
			}

			if (modelID != nullptr)
			{
				builder.withModelID(modelID);
			}

			if (beaconCacheMaxRecordAge >= 1)
			{
				builder.withBeaconCacheMaxRecordAge(beaconCacheMaxRecordAge);
			}

			if (beaconCacheLowerMemoryBoundary >= 1)
			{
				builder.withBeaconCacheLowerMemoryBoundary(beaconCacheLowerMemoryBoundary);
			}

			if (beaconCacheUpperMemoryBoundary >= 1)
			{
				builder.withBeaconCacheUpperMemoryBoundary(beaconCacheUpperMemoryBoundary);
			}

			std::shared_ptr<openkit::IOpenKit> openKit = builder.build();

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new OpenKitHandle();
			handle->sharedPointer = openKit;
			handle->logger = loggerHandle->logger;
		}
		CATCH_AND_LOG(loggerHandle)
		return handle;
	}

	void shutdownOpenKit(OpenKitHandle* openKitHandle)
	{
		// Sanity
		if (openKitHandle == nullptr)
		{
			return;
		}

		TRY
		{
			// retrieve the OpenKit instance from the handle and call the respective method
			assert(openKitHandle->sharedPointer != nullptr);
			openKitHandle->sharedPointer->shutdown();

			// release shared pointer
			openKitHandle->sharedPointer = nullptr;
			openKitHandle->logger = nullptr;
			openKitHandle->trustManager = nullptr;
			delete openKitHandle;
		}
		CATCH_AND_LOG(openKitHandle)
	}

	bool waitForInitCompletion(struct OpenKitHandle* openKitHandle)
	{
		TRY
		{
			if (openKitHandle)
			{
				// retrieve the OpenKit instance from the handle and call the respective method
				assert(openKitHandle->sharedPointer != nullptr);
				return openKitHandle->sharedPointer->waitForInitCompletion();
			}
		}
		CATCH_AND_LOG(openKitHandle)
		return false;
	}

	bool waitForInitCompletionWithTimeout(struct OpenKitHandle* openKitHandle, int64_t timeoutMillis)
	{
		TRY
		{
			if (openKitHandle)
			{
				// retrieve the OpenKit instance from the handle and call the respective method
				assert(openKitHandle->sharedPointer != nullptr);
				openKitHandle->sharedPointer->waitForInitCompletion(timeoutMillis);
			}
		}
		CATCH_AND_LOG(openKitHandle)
		return false;
	}

	bool isInitialized(struct OpenKitHandle* openKitHandle)
	{
		TRY
		{
			if (openKitHandle)
			{
				// retrieve the OpenKit instance from the handle and call the respective method
				assert(openKitHandle->sharedPointer != nullptr);
				return openKitHandle->sharedPointer->isInitialized();
			}
		}
		CATCH_AND_LOG(openKitHandle)
		return false;
	}

	//--------------
	//  Session
	//--------------

	typedef struct SessionHandle
	{
		std::shared_ptr<openkit::ISession> sharedPointer = nullptr;
		std::shared_ptr<openkit::ILogger> logger = nullptr;
	} SessionHandle;

	SessionHandle* createSession(OpenKitHandle* openKitHandle, const char* clientIPAddress)
	{
		// Sanity
		if (openKitHandle == nullptr)
		{
			return nullptr;
		}

		SessionHandle* handle = nullptr;
		TRY
		{
			// retrieve the OpenKit instance from the handle and call the respective method
			assert(openKitHandle->sharedPointer != nullptr);
			std::shared_ptr<openkit::ISession> session = openKitHandle->sharedPointer->createSession(clientIPAddress);

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new SessionHandle();
			handle->sharedPointer = session;
			handle->logger = openKitHandle->logger;
		}
		CATCH_AND_LOG(openKitHandle)
		return handle;
	}

	void endSession(SessionHandle* sessionHandle)
	{
		// Sanity
		if (sessionHandle == nullptr)
		{
			return;
		}

		TRY
		{
			// retrieve the Session instance from the handle and call the respective method
			assert(sessionHandle->sharedPointer != nullptr);
			sessionHandle->sharedPointer->end();

			// release shared pointer
			sessionHandle->sharedPointer = nullptr;
			sessionHandle->logger = nullptr;
			delete sessionHandle;
		}
		CATCH_AND_LOG(sessionHandle)
	}

	void identifyUser(SessionHandle* sessionHandle, const char* userTag)
	{
		TRY
		{
			if (sessionHandle)
			{
				// retrieve the Session instance from the handle and call the respective method
				assert(sessionHandle->sharedPointer != nullptr);
				sessionHandle->sharedPointer->identifyUser(userTag);
			}
		}
		CATCH_AND_LOG(sessionHandle)
	}

	void reportCrash(SessionHandle* sessionHandle, const char* errorName, const char* reason, const char* stacktrace)
	{
		TRY
		{
			if (sessionHandle)
			{
				// retrieve the Session instance from the handle and call the respective method
				assert(sessionHandle->sharedPointer != nullptr);
				sessionHandle->sharedPointer->reportCrash(errorName, reason, stacktrace);
			}
		}
		CATCH_AND_LOG(sessionHandle)
	}

	//--------------
	//  Root Action
	//--------------

	typedef struct RootActionHandle
	{
		std::shared_ptr<openkit::IRootAction> sharedPointer = nullptr;
		std::shared_ptr<openkit::ILogger> logger = nullptr;
	} RootActionHandle;

	RootActionHandle* enterRootAction(SessionHandle* sessionHandle, const char* rootActionName)
	{
		// Sanity
		if (sessionHandle == nullptr)
		{
			return nullptr;
		}

		RootActionHandle* handle = nullptr;
		TRY
		{
			// retrieve the Session instance from the handle and call the respective method
			assert(sessionHandle->sharedPointer != nullptr);
			std::shared_ptr<openkit::IRootAction> rootAction = sessionHandle->sharedPointer->enterAction(rootActionName);

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new RootActionHandle();
			handle->sharedPointer = rootAction;
			handle->logger = sessionHandle->logger;
		}
		CATCH_AND_LOG(sessionHandle)
		return handle;
	}

	void leaveRootAction(RootActionHandle* rootActionHandle)
	{
		// Sanity
		if (rootActionHandle == nullptr)
		{
			return;
		}

		TRY
		{
			// retrieve the RootAction instance from the handle and call the respective method
			assert(rootActionHandle->sharedPointer != nullptr);
			rootActionHandle->sharedPointer->leaveAction();

			// release shared pointer
			rootActionHandle->sharedPointer = nullptr;
			rootActionHandle->logger = nullptr;
			delete rootActionHandle;
		}
		CATCH_AND_LOG(rootActionHandle)
	}

	void reportEventOnRootAction(RootActionHandle* rootActionHandle, const char* eventName)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportEvent(eventName);
			}
		}
		CATCH_AND_LOG(rootActionHandle)
	}

	void reportIntValueOnRootAction(RootActionHandle* rootActionHandle, const char* valueName, int32_t value)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(rootActionHandle)
	}

	void reportDoubleValueOnRootAction(RootActionHandle* rootActionHandle, const char* valueName, double value)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(rootActionHandle)
	}

	void reportStringValueOnRootAction(RootActionHandle* rootActionHandle, const char* valueName, const char* value)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(rootActionHandle)
	}

	void reportErrorOnRootAction(RootActionHandle* rootActionHandle, const char* errorName, int32_t errorCode, const char* reason)
	{
		TRY
		{
			if (rootActionHandle)
			{
				// retrieve the RootAction instance from the handle and call the respective method
				assert(rootActionHandle->sharedPointer != nullptr);
				rootActionHandle->sharedPointer->reportError(errorName, errorCode, reason);
			}
		}
		CATCH_AND_LOG(rootActionHandle)
	}


	//--------------
	//  Action
	//--------------

	typedef struct ActionHandle
	{
		std::shared_ptr<openkit::IAction> sharedPointer = nullptr;
		std::shared_ptr<openkit::ILogger> logger = nullptr;
	} ActionHandle;

	ActionHandle* enterAction(RootActionHandle* rootActionHandle, const char* actionName)
	{
		// Sanity
		if (rootActionHandle == nullptr)
		{
			return nullptr;
		}

		ActionHandle* handle = nullptr;
		TRY
		{
			// retrieve the RootAction instance from the handle and call the respective method
			assert(rootActionHandle->sharedPointer != nullptr);
			std::shared_ptr<openkit::IAction> action = rootActionHandle->sharedPointer->enterAction(actionName);
		
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new ActionHandle();
			handle->sharedPointer = action;
			handle->logger = rootActionHandle->logger;
		}
		CATCH_AND_LOG(rootActionHandle)
		return handle;
	}

	void leaveAction(ActionHandle* actionHandle)
	{
		// Sanity
		if (actionHandle == nullptr)
		{
			return;
		}

		TRY
		{
			// retrieve the Action instance from the handle and call the respective method
			assert(actionHandle->sharedPointer != nullptr);
			actionHandle->sharedPointer->leaveAction();

			// release shared pointer
			actionHandle->sharedPointer = nullptr;
			actionHandle->logger = nullptr;
			delete actionHandle;
		}
		CATCH_AND_LOG(actionHandle)
	}

	void reportEventOnAction(ActionHandle* actionHandle, const char* eventName)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportEvent(eventName);
			}
		}
		CATCH_AND_LOG(actionHandle)
	}

	void reportIntValueOnAction(ActionHandle* actionHandle, const char* valueName, int32_t value)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(actionHandle)
	}

	void reportDoubleValueOnAction(ActionHandle* actionHandle, const char* valueName, double value)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(actionHandle)
	}
	
	void reportStringValueOnAction(ActionHandle* actionHandle, const char* valueName, const char* value)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportValue(valueName, value);
			}
		}
		CATCH_AND_LOG(actionHandle)
	}

	void reportErrorOnAction(ActionHandle* actionHandle, const char* errorName, int32_t errorCode, const char* reason)
	{
		TRY
		{
			if (actionHandle)
			{
				// retrieve the Action instance from the handle and call the respective method
				assert(actionHandle->sharedPointer != nullptr);
				actionHandle->sharedPointer->reportError(errorName, errorCode, reason);
			}
		}
		CATCH_AND_LOG(actionHandle)
	}


	//--------------------
	//  Webrequest Tracer
	//--------------------

	typedef struct WebRequestTracerHandle
	{
		std::shared_ptr<openkit::IWebRequestTracer> sharedPointer = nullptr;
		std::shared_ptr<openkit::ILogger> logger = nullptr;
	} WebRequestTracerHandle;

	WebRequestTracerHandle* traceWebRequestOnRootAction(RootActionHandle* rootActionHandle, const char* url)
	{
		// Sanity
		if (rootActionHandle == nullptr)
		{
			return nullptr;
		}

		WebRequestTracerHandle* handle = nullptr;
		TRY
		{
			// retrieve the RootAction instance from the handle and call the respective method
			assert(rootActionHandle->sharedPointer != nullptr);
			auto traceWebRequest = rootActionHandle->sharedPointer->traceWebRequest(url);

			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new WebRequestTracerHandle();
			handle->sharedPointer = traceWebRequest;
			handle->logger = rootActionHandle->logger;
		}
		CATCH_AND_LOG(rootActionHandle)
		return handle;
	}

	WebRequestTracerHandle* traceWebRequestOnAction(ActionHandle* actionHandle, const char* url)
	{
		// Sanity
		if (actionHandle == nullptr)
		{
			return nullptr;
		}

		WebRequestTracerHandle* handle = nullptr;
		TRY
		{
			// retrieve the Action instance from the handle and call the respective method
			assert(actionHandle->sharedPointer != nullptr);
			auto traceWebRequest = actionHandle->sharedPointer->traceWebRequest(url);
			
			// storing the returned shared pointer in the handle prevents it from going out of scope
			handle = new WebRequestTracerHandle();
			handle->sharedPointer = traceWebRequest;
			handle->logger = actionHandle->logger;
		}
		CATCH_AND_LOG(actionHandle)
		return handle;
	}

	void startWebRequest(WebRequestTracerHandle* webRequestTracerHandle)
	{
		TRY
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				webRequestTracerHandle->sharedPointer->start();
			}
		}
		CATCH_AND_LOG(webRequestTracerHandle)
	}

	void stopWebRequest(WebRequestTracerHandle* webRequestTracerHandle)
	{
		// Sanity
		if (webRequestTracerHandle == nullptr)
		{
			return;
		}

		TRY
		{
			// retrieve the WebRequestTracer instance from the handle and call the respective method
			assert(webRequestTracerHandle->sharedPointer != nullptr);
			webRequestTracerHandle->sharedPointer->stop();

			// release shared pointer
			webRequestTracerHandle->sharedPointer = nullptr;
			webRequestTracerHandle->logger = nullptr;
			delete webRequestTracerHandle;
		}
		CATCH_AND_LOG(webRequestTracerHandle)
	}

	const char* getTag(WebRequestTracerHandle* webRequestTracerHandle)
	{
		TRY
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				return webRequestTracerHandle->sharedPointer->getTag();
			}
		}
		CATCH_AND_LOG(webRequestTracerHandle)
		return nullptr;
	}

	void setResponseCode(WebRequestTracerHandle* webRequestTracerHandle, int32_t responseCode)
	{
		TRY
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				webRequestTracerHandle->sharedPointer->setResponseCode(responseCode);
			}
		}
		CATCH_AND_LOG(webRequestTracerHandle)
	}

	void setBytesSent(WebRequestTracerHandle* webRequestTracerHandle, int32_t bytesSent)
	{
		TRY
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				webRequestTracerHandle->sharedPointer->setBytesSent(bytesSent);
			}
		}
		CATCH_AND_LOG(webRequestTracerHandle)
	}

	void setBytesReceived(WebRequestTracerHandle* webRequestTracerHandle, int32_t bytesReceived)
	{
		TRY
		{
			if (webRequestTracerHandle)
			{
				// retrieve the WebRequestTracer instance from the handle and call the respective method
				assert(webRequestTracerHandle->sharedPointer != nullptr);
				webRequestTracerHandle->sharedPointer->setBytesReceived(bytesReceived);
			}
		}
		CATCH_AND_LOG(webRequestTracerHandle)
	}

} /* extern "C" */