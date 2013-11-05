/*
 *	PowerSupplyStateMachine.h
 *	!CHOAS
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#ifndef PowerSupply_PowerSupplyStateMachine_h
#define PowerSupply_PowerSupplyStateMachine_h

//used to enlarge the statically created vector
#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 30
#define BOOST_MPL_LIMIT_MAP_SIZE 30

// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/internal_row.hpp>
#include <boost/msm/front/euml/common.hpp>
// for And_ operator
#include <boost/msm/front/euml/operator.hpp>

namespace driver {
	namespace powersupply {
		//--------------------------- events ------------------------------
		//SM Event
		namespace PowersupplyEventType {
			
			//! the powersupply is found on on startup of the control unit or during check
			struct turned_on {};
			
			//! Event forwarded on allarm presence or command error
			struct fault {};
			//! reset the fault state
			struct reset {};
			
			enum ModeStart
			{
				ModeStartToOperational=0,
				ModeStartToStandby=1
			};
			
			//! identity the start of the "mode"{oper(true),stby(false)} command
			struct mode_start{
				mode_start(ModeStart _direction):direction(_direction){}
				
				ModeStart direction;
			};
			struct mode_end{};
			
			//! identity the start of the "slope" command
			struct sslp_start{};
			struct sslp_end{};
			
			//! identity the start of the "set" command
			struct sett_start{};
			struct sett_end{};
			
			//! identity the start of the "powr" command
			struct powr_start{};
			struct powr_end{};
		}
		
		//--------------------------- state ------------------------------
		//! Non working state
		struct OFF : public boost::msm::front::state<>{};
		struct STANDBY : public boost::msm::front::state<> {};
		struct OPERATIONAL : public boost::msm::front::state<> {};
		struct FAULTY : public boost::msm::front::state<> {};
		
		//! Command proprietary state
		struct MODE_STBY_RUN : public boost::msm::front::state<> {};
		struct MODE_OPER_RUN : public boost::msm::front::state<> {};
		struct SSLP_STBY_RUN : public boost::msm::front::state<> {};
		struct SSLP_OPER_RUN : public boost::msm::front::state<> {};
		struct SETT_STBY_RUN : public boost::msm::front::state<> {};
		struct SETT_OPER_RUN : public boost::msm::front::state<> {};
		struct POWR_RUN : public boost::msm::front::state<> {};
		
		//--------------------------- MSM definition ------------------------------
		
		//! Boost state machine for powersupply amnagment
		struct powersupply_state_machine_impl : public boost::msm::front::state_machine_def<powersupply_state_machine_impl> {
			struct mode_stby_guard
			{
				template <class EVT,class FSM,class SourceState,class TargetState>
				bool operator()(EVT const& evt ,FSM&,SourceState& ,TargetState& )
				{
					std::cout << "evt.direction | PowersupplyEventType::ModeStartToOperational" << (evt.direction == PowersupplyEventType::ModeStartToOperational) << std::endl;
					return evt.direction == PowersupplyEventType::ModeStartToOperational;
				}
			};
			struct mode_oper_guard
			{
				template <class EVT,class FSM,class SourceState,class TargetState>
				bool operator()(EVT const& evt ,FSM&,SourceState& ,TargetState& )
				{
					std::cout << "evt.direction | PowersupplyEventType::ModeStartToStandby" << (evt.direction == PowersupplyEventType::ModeStartToStandby) << std::endl;
					return evt.direction == PowersupplyEventType::ModeStartToStandby;
				}
			};
			
			//! the initial state of the player SM. Must be defined
			typedef OFF initial_state;
			
			//non working state
			typedef boost::msm::front::Row <  OFF,				PowersupplyEventType::turned_on,	STANDBY, boost::msm::front::none , boost::msm::front::none >	off_to_standby;
			typedef boost::msm::front::Row <  STANDBY,			PowersupplyEventType::fault,		FAULTY, boost::msm::front::none , boost::msm::front::none >		standby_to_faulty;
			typedef boost::msm::front::Row <  OPERATIONAL,		PowersupplyEventType::fault,		STANDBY, boost::msm::front::none , boost::msm::front::none >	operational_to_faulty;
			typedef boost::msm::front::Row <  FAULTY,			PowersupplyEventType::reset,		STANDBY, boost::msm::front::none , boost::msm::front::none >	faulty_to_standby;
			
			//mode command form standby
			typedef boost::msm::front::Row <  STANDBY,			PowersupplyEventType::mode_start,	MODE_STBY_RUN, boost::msm::front::none , mode_stby_guard >			standby_to_mode_stdby_run;
			typedef boost::msm::front::Row <  MODE_STBY_RUN,	PowersupplyEventType::mode_end,		OPERATIONAL, boost::msm::front::none , boost::msm::front::none >	mode_stdby_run_to_operation;
			typedef boost::msm::front::Row <  MODE_STBY_RUN,	PowersupplyEventType::fault,		FAULTY, boost::msm::front::none , boost::msm::front::none >			mode_stdby_run_to_faulty;
			
			//mode command form operational
			typedef boost::msm::front::Row <  OPERATIONAL,		PowersupplyEventType::mode_start,	MODE_OPER_RUN, boost::msm::front::none , mode_oper_guard >			standby_to_mode_oper_run;
			typedef boost::msm::front::Row <  MODE_OPER_RUN,	PowersupplyEventType::mode_end,		OPERATIONAL, boost::msm::front::none , boost::msm::front::none >	mode_oper_run_to_operation;
			typedef boost::msm::front::Row <  MODE_OPER_RUN,	PowersupplyEventType::fault,		FAULTY, boost::msm::front::none , boost::msm::front::none >			mode_oper_run_to_faulty;
			
			//sslp command from standby
			typedef boost::msm::front::Row <  STANDBY,			PowersupplyEventType::sslp_start,	SSLP_STBY_RUN, boost::msm::front::none , boost::msm::front::none >	standby_to_sslp_stby_run;
			typedef boost::msm::front::Row <  SSLP_STBY_RUN,	PowersupplyEventType::sslp_end,		STANDBY, boost::msm::front::none , boost::msm::front::none >		sslp_stby_run_to_standby;
			typedef boost::msm::front::Row <  SSLP_STBY_RUN,	PowersupplyEventType::fault,		FAULTY, boost::msm::front::none , boost::msm::front::none >			sslp_stby_run_to_faulty;
			
			//sslp command from operational
			typedef boost::msm::front::Row <  OPERATIONAL,		PowersupplyEventType::sslp_start,	OPERATIONAL, boost::msm::front::none , boost::msm::front::none >	operational_to_sslp_oper_run;
			typedef boost::msm::front::Row <  SSLP_OPER_RUN,	PowersupplyEventType::sslp_end,		OPERATIONAL, boost::msm::front::none , boost::msm::front::none >	sslp_oper_run_to_operational;
			typedef boost::msm::front::Row <  SSLP_OPER_RUN,	PowersupplyEventType::fault,		FAULTY, boost::msm::front::none , boost::msm::front::none >			sslp_oper_run_to_faulty;
			
			//sett command from standby
			typedef boost::msm::front::Row <  STANDBY,			PowersupplyEventType::sett_start,	SETT_STBY_RUN, boost::msm::front::none , boost::msm::front::none >	standby_to_sett_stby_run;
			typedef boost::msm::front::Row <  SETT_STBY_RUN,	PowersupplyEventType::sett_end,		STANDBY, boost::msm::front::none , boost::msm::front::none >		sett_stby_run_to_standby;
			typedef boost::msm::front::Row <  SETT_STBY_RUN,	PowersupplyEventType::fault,		FAULTY, boost::msm::front::none , boost::msm::front::none >			sett_stby_run_to_faulty;
			
			//sett command from operational
			typedef boost::msm::front::Row <  OPERATIONAL,		PowersupplyEventType::sett_start,	OPERATIONAL, boost::msm::front::none , boost::msm::front::none >	operational_to_sett_oper_run;
			typedef boost::msm::front::Row <  SETT_OPER_RUN,	PowersupplyEventType::sett_end,		OPERATIONAL, boost::msm::front::none , boost::msm::front::none >	sett_oper_run_to_operational;
			typedef boost::msm::front::Row <  SETT_OPER_RUN,	PowersupplyEventType::fault,		FAULTY, boost::msm::front::none , boost::msm::front::none >			sett_oper_run_to_faulty;
			
			
			// Transition table for initialization services
			struct transition_table : boost::mpl::vector<
			off_to_standby, standby_to_faulty, operational_to_faulty, faulty_to_standby,
			standby_to_mode_stdby_run, mode_stdby_run_to_operation, mode_stdby_run_to_faulty,
			standby_to_mode_oper_run, mode_oper_run_to_operation, mode_oper_run_to_faulty,
			standby_to_sslp_stby_run, sslp_stby_run_to_standby, sslp_stby_run_to_faulty,
			operational_to_sslp_oper_run, sslp_oper_run_to_operational, sslp_oper_run_to_faulty,
			standby_to_sett_stby_run, sett_stby_run_to_standby, sett_stby_run_to_faulty,
			operational_to_sett_oper_run, sett_oper_run_to_operational, sett_oper_run_to_faulty> {};
			
			template <class FSM,class Event>
			void no_transition(Event const& ,FSM&, int ) {}
		};
		
		inline const char * get_state_name(int state_id) {
			switch(state_id) {
				case 0:
					return "Off";
				case 1:
					return "Standby";
				case 2:
					return "Operational";
				case 3:
					return "Faulty";
				case 4:
					return "Mode cmd stanby";
				case 5:
					return "Mode cmd operational";
				case 6:
					return "SSLP cmd standby";
				case 7:
					return "SSLP cmd operational";
				case 8:
					return "SETT cmd standby";
				case 9:
					return "SETT cmd operational";
					
			}
			return "Wrong command index";
		};
	}
}
#endif
