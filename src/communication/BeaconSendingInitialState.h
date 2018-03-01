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

#ifndef _COMMUNICATION_BEACONSENDINGINITIALSTATE_H
#define _COMMUNICATION_BEACONSENDINGINITIALSTATE_H

#include "communication/AbstractBeaconSendingState.h"

#include <vector>
#include <chrono>

namespace communication
{
	///
	/// Initial state for beacon sending.
	/// NOTE: The initial state is used to retrieve the configuration from the server and update the configuration.
	///
	class BeaconSendingInitialState : public AbstractBeaconSendingState
	{
	public:
		///
		/// Constructor
		///
		BeaconSendingInitialState();

		///
		/// Destructor
		///
		virtual ~BeaconSendingInitialState() {};

		///
		/// Execute this state
		/// @param context Instance of the BeaconSendingContext responsible for state transitions
		///
		virtual void doExecute(BeaconSendingContext& context) override;

		///
		/// Return the shutdown state
		/// @returns an instance of @c AbstractBeaconSendingState handling the shutdown after the init state
		///
		virtual std::shared_ptr<AbstractBeaconSendingState> getShutdownState() override;

		///
		/// Return a flag if this is a shutdown state.
		/// @return @c true if this state is a shutdown state, @c false if not
		///
		virtual bool isAShutdownState() override;

		static const std::chrono::milliseconds INITIAL_RETRY_SLEEP_TIME_MILLISECONDS;
		static const std::vector<std::chrono::milliseconds> REINIT_DELAY_MILLISECONDS;

	private:

		///
		/// Index to re-initialize delays
		///
		uint32_t mReinitializeDelayIndex;
	};
}
#endif