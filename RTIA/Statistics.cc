// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003-2005  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------

#include <config.h>
#include "Statistics.hh"

#include <cstdlib>
#include <iostream>
#include <string>

using std::cout ;
using std::endl ;
using std::string ;
using std::ostream ;
using std::endl ;
using std::vector ;

#define STRINGIZE(m) #m
#define FED_MSG_NAME(msg_) fedMessageName[msg_] = STRINGIZE(msg_)
#define RTI_MSG_NAME(msg_) rtiMessageName[msg_] = STRINGIZE(msg_)
 
namespace certi {
namespace rtia {

bool Statistics::initialized = false ;
vector<string> Statistics::fedMessageName(Message::LAST);
vector<string> Statistics::rtiMessageName(NetworkMessage::LAST);

// ----------------------------------------------------------------------------
//! Initialize the two sets used for collecting number of messages exchanged.
Statistics::Statistics()
    : federateServiceSet(Message::LAST, 0),
      rtiServiceSet(NetworkMessage::LAST, 0),
      myDisplay(true), myDisplayZero(false)
{
    if (getenv("CERTI_NO_STATISTICS"))
	myDisplay = false ;

    if (!initialized) {
        FED_MSG_NAME(Message::OPEN_CONNEXION);
        FED_MSG_NAME(Message::CLOSE_CONNEXION);

        // Federation
	FED_MSG_NAME(Message::CREATE_FEDERATION_EXECUTION);
	FED_MSG_NAME(Message::DESTROY_FEDERATION_EXECUTION);
	FED_MSG_NAME(Message::JOIN_FEDERATION_EXECUTION);
	FED_MSG_NAME(Message::RESIGN_FEDERATION_EXECUTION);
	FED_MSG_NAME(Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT);
	FED_MSG_NAME(Message::SYNCHRONIZATION_POINT_REGISTRATION_FAILED);
	FED_MSG_NAME(Message::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED);
	FED_MSG_NAME(Message::ANNOUNCE_SYNCHRONIZATION_POINT);
	FED_MSG_NAME(Message::SYNCHRONIZATION_POINT_ACHIEVED);
	FED_MSG_NAME(Message::FEDERATION_SYNCHRONIZED);
	FED_MSG_NAME(Message::REQUEST_FEDERATION_SAVE);
	FED_MSG_NAME(Message::INITIATE_FEDERATE_SAVE);
	FED_MSG_NAME(Message::FEDERATE_SAVE_BEGUN);
	FED_MSG_NAME(Message::FEDERATE_SAVE_COMPLETE);
	FED_MSG_NAME(Message::FEDERATE_SAVE_NOT_COMPLETE);
	FED_MSG_NAME(Message::FEDERATION_SAVED);
	FED_MSG_NAME(Message::FEDERATION_NOT_SAVED);
	FED_MSG_NAME(Message::REQUEST_FEDERATION_RESTORE);
	FED_MSG_NAME(Message::REQUEST_FEDERATION_RESTORE_FAILED);
	FED_MSG_NAME(Message::REQUEST_FEDERATION_RESTORE_SUCCEEDED);
	FED_MSG_NAME(Message::INITIATE_FEDERATE_RESTORE);
	FED_MSG_NAME(Message::FEDERATE_RESTORE_COMPLETE);
	FED_MSG_NAME(Message::FEDERATE_RESTORE_NOT_COMPLETE);
	FED_MSG_NAME(Message::FEDERATION_RESTORED);
	FED_MSG_NAME(Message::FEDERATION_NOT_RESTORED);
	FED_MSG_NAME(Message::FEDERATION_RESTORE_BEGUN);

	// Declaration
	FED_MSG_NAME(Message::PUBLISH_OBJECT_CLASS);
	FED_MSG_NAME(Message::UNPUBLISH_OBJECT_CLASS);
	FED_MSG_NAME(Message::PUBLISH_INTERACTION_CLASS);
	FED_MSG_NAME(Message::UNPUBLISH_INTERACTION_CLASS);
	FED_MSG_NAME(Message::SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES);
	FED_MSG_NAME(Message::UNSUBSCRIBE_OBJECT_CLASS);
	FED_MSG_NAME(Message::SUBSCRIBE_INTERACTION_CLASS);
	FED_MSG_NAME(Message::UNSUBSCRIBE_INTERACTION_CLASS);
	FED_MSG_NAME(Message::START_REGISTRATION_FOR_OBJECT_CLASS);
	FED_MSG_NAME(Message::STOP_REGISTRATION_FOR_OBJECT_CLASS);
	FED_MSG_NAME(Message::TURN_INTERACTIONS_ON);
	FED_MSG_NAME(Message::TURN_INTERACTIONS_OFF);

	// Object
	FED_MSG_NAME(Message::REGISTER_OBJECT_INSTANCE);
	FED_MSG_NAME(Message::UPDATE_ATTRIBUTE_VALUES);
	FED_MSG_NAME(Message::DISCOVER_OBJECT_INSTANCE);
	FED_MSG_NAME(Message::REFLECT_ATTRIBUTE_VALUES);
	FED_MSG_NAME(Message::SEND_INTERACTION);
	FED_MSG_NAME(Message::RECEIVE_INTERACTION);
	FED_MSG_NAME(Message::DELETE_OBJECT_INSTANCE);
	FED_MSG_NAME(Message::LOCAL_DELETE_OBJECT_INSTANCE);
	FED_MSG_NAME(Message::REMOVE_OBJECT_INSTANCE);
	FED_MSG_NAME(Message::CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE);
	FED_MSG_NAME(Message::CHANGE_INTERACTION_TRANSPORTATION_TYPE);
	FED_MSG_NAME(Message::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE);
	FED_MSG_NAME(Message::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE);
	FED_MSG_NAME(Message::PROVIDE_ATTRIBUTE_VALUE_UPDATE);
	FED_MSG_NAME(Message::ATTRIBUTES_IN_SCOPE);
	FED_MSG_NAME(Message::ATTRIBUTES_OUT_OF_SCOPE);
	FED_MSG_NAME(Message::TURN_UPDATES_ON_FOR_OBJECT_INSTANCE);
	FED_MSG_NAME(Message::TURN_UPDATES_OFF_FOR_OBJECT_INSTANCE);

	// Ownership
	FED_MSG_NAME(Message::REQUEST_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
	FED_MSG_NAME(Message::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION);
	FED_MSG_NAME(Message::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
	FED_MSG_NAME(Message::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION);
	FED_MSG_NAME(Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION);
	FED_MSG_NAME(Message::REQUEST_ATTRIBUTE_OWNERSHIP_ACQUISITION);
	FED_MSG_NAME(Message::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE);
	FED_MSG_NAME(Message::QUERY_ATTRIBUTE_OWNERSHIP);
	FED_MSG_NAME(Message::INFORM_ATTRIBUTE_OWNERSHIP);
	FED_MSG_NAME(Message::IS_ATTRIBUTE_OWNED_BY_FEDERATE);
	FED_MSG_NAME(Message::ATTRIBUTE_IS_NOT_OWNED);
	FED_MSG_NAME(Message::ATTRIBUTE_OWNED_BY_RTI);
	FED_MSG_NAME(Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE);
	FED_MSG_NAME(Message::ATTRIBUTE_OWNERSHIP_UNAVAILABLE);
	FED_MSG_NAME(Message::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
	FED_MSG_NAME(Message::ATTRIBUTE_OWNERSHIP_ACQUISITION);
	FED_MSG_NAME(Message::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
	FED_MSG_NAME(Message::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE);
	FED_MSG_NAME(Message::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION);
	FED_MSG_NAME(Message::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION);

	// Time
	FED_MSG_NAME(Message::CHANGE_ATTRIBUTE_ORDER_TYPE);
	FED_MSG_NAME(Message::CHANGE_INTERACTION_ORDER_TYPE);
	FED_MSG_NAME(Message::ENABLE_TIME_REGULATION);
	FED_MSG_NAME(Message::DISABLE_TIME_REGULATION);
	FED_MSG_NAME(Message::ENABLE_TIME_CONSTRAINED);
	FED_MSG_NAME(Message::DISABLE_TIME_CONSTRAINED);
	FED_MSG_NAME(Message::QUERY_LBTS);
	FED_MSG_NAME(Message::QUERY_FEDERATE_TIME);
	FED_MSG_NAME(Message::QUERY_MIN_NEXT_EVENT_TIME);
	FED_MSG_NAME(Message::MODIFY_LOOKAHEAD);
	FED_MSG_NAME(Message::QUERY_LOOKAHEAD);
	FED_MSG_NAME(Message::RETRACT);
	FED_MSG_NAME(Message::REQUEST_RETRACTION);
	FED_MSG_NAME(Message::TIME_ADVANCE_REQUEST);
	FED_MSG_NAME(Message::TIME_ADVANCE_REQUEST_AVAILABLE);
	FED_MSG_NAME(Message::NEXT_EVENT_REQUEST);
	FED_MSG_NAME(Message::NEXT_EVENT_REQUEST_AVAILABLE);
	FED_MSG_NAME(Message::FLUSH_QUEUE_REQUEST);
	FED_MSG_NAME(Message::TIME_ADVANCE_GRANT);
	FED_MSG_NAME(Message::ENABLE_ASYNCHRONOUS_DELIVERY);
	FED_MSG_NAME(Message::DISABLE_ASYNCHRONOUS_DELIVERY);
	FED_MSG_NAME(Message::TIME_REGULATION_ENABLED);
	FED_MSG_NAME(Message::TIME_CONSTRAINED_ENABLED);

	// Data Distribution
	FED_MSG_NAME(Message::DDM_CREATE_REGION);
	FED_MSG_NAME(Message::DDM_MODIFY_REGION);
	FED_MSG_NAME(Message::DDM_DELETE_REGION);
	FED_MSG_NAME(Message::DDM_REGISTER_OBJECT);
	FED_MSG_NAME(Message::DDM_ASSOCIATE_REGION);
	FED_MSG_NAME(Message::DDM_UNASSOCIATE_REGION);
	FED_MSG_NAME(Message::DDM_SUBSCRIBE_ATTRIBUTES);
	FED_MSG_NAME(Message::DDM_UNSUBSCRIBE_ATTRIBUTES);
	FED_MSG_NAME(Message::DDM_SUBSCRIBE_INTERACTION);
	FED_MSG_NAME(Message::DDM_UNSUBSCRIBE_INTERACTION);
	FED_MSG_NAME(Message::DDM_REQUEST_UPDATE);

	// Support Services
	FED_MSG_NAME(Message::GET_OBJECT_CLASS_HANDLE);
	FED_MSG_NAME(Message::GET_OBJECT_CLASS_NAME);
	FED_MSG_NAME(Message::GET_ATTRIBUTE_HANDLE);
	FED_MSG_NAME(Message::GET_ATTRIBUTE_NAME);
	FED_MSG_NAME(Message::GET_INTERACTION_CLASS_HANDLE);
	FED_MSG_NAME(Message::GET_INTERACTION_CLASS_NAME);
	FED_MSG_NAME(Message::GET_PARAMETER_HANDLE);
	FED_MSG_NAME(Message::GET_PARAMETER_NAME);
	FED_MSG_NAME(Message::GET_OBJECT_INSTANCE_HANDLE);
	FED_MSG_NAME(Message::GET_OBJECT_INSTANCE_NAME);
	FED_MSG_NAME(Message::GET_SPACE_HANDLE);
	FED_MSG_NAME(Message::GET_SPACE_NAME);
	FED_MSG_NAME(Message::GET_DIMENSION_HANDLE);
	FED_MSG_NAME(Message::GET_DIMENSION_NAME);
	FED_MSG_NAME(Message::GET_ATTRIBUTE_SPACE_HANDLE);
	FED_MSG_NAME(Message::GET_OBJECT_CLASS);
	FED_MSG_NAME(Message::GET_INTERACTION_SPACE_HANDLE);
	FED_MSG_NAME(Message::GET_TRANSPORTATION_HANDLE);
	FED_MSG_NAME(Message::GET_TRANSPORTATION_NAME);
	FED_MSG_NAME(Message::GET_ORDERING_HANDLE);
	FED_MSG_NAME(Message::GET_ORDERING_NAME);
	FED_MSG_NAME(Message::ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH);
	FED_MSG_NAME(Message::DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH);
	FED_MSG_NAME(Message::ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH);
	FED_MSG_NAME(Message::DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH);
	FED_MSG_NAME(Message::ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH);
	FED_MSG_NAME(Message::DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH);
	FED_MSG_NAME(Message::ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH);
	FED_MSG_NAME(Message::DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH);
	FED_MSG_NAME(Message::TICK_REQUEST);
	FED_MSG_NAME(Message::TICK_REQUEST_NEXT);
	FED_MSG_NAME(Message::TICK_REQUEST_STOP);

	FED_MSG_NAME(Message::RESERVE_OBJECT_INSTANCE_NAME);
	FED_MSG_NAME(Message::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED);
	FED_MSG_NAME(Message::RESERVE_OBJECT_INSTANCE_NAME_FAILED);

	// RTI messages
	RTI_MSG_NAME(NetworkMessage::CLOSE_CONNEXION);
	RTI_MSG_NAME(NetworkMessage::MESSAGE_NULL);
	RTI_MSG_NAME(NetworkMessage::CREATE_FEDERATION_EXECUTION);
	RTI_MSG_NAME(NetworkMessage::DESTROY_FEDERATION_EXECUTION);
	RTI_MSG_NAME(NetworkMessage::JOIN_FEDERATION_EXECUTION);
	RTI_MSG_NAME(NetworkMessage::RESIGN_FEDERATION_EXECUTION);
	RTI_MSG_NAME(NetworkMessage::SET_TIME_REGULATING);
	RTI_MSG_NAME(NetworkMessage::SET_TIME_CONSTRAINED);
	RTI_MSG_NAME(NetworkMessage::TIME_REGULATION_ENABLED);
	RTI_MSG_NAME(NetworkMessage::TIME_CONSTRAINED_ENABLED);
	RTI_MSG_NAME(NetworkMessage::REGISTER_FEDERATION_SYNCHRONIZATION_POINT);
	RTI_MSG_NAME(NetworkMessage::CONFIRM_SYNCHRONIZATION_POINT_REGISTRATION);
	RTI_MSG_NAME(NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT);
	RTI_MSG_NAME(NetworkMessage::SYNCHRONIZATION_POINT_ACHIEVED);
	RTI_MSG_NAME(NetworkMessage::FEDERATION_SYNCHRONIZED);
	RTI_MSG_NAME(NetworkMessage::REQUEST_FEDERATION_SAVE);
	RTI_MSG_NAME(NetworkMessage::FEDERATE_SAVE_BEGUN);
	RTI_MSG_NAME(NetworkMessage::FEDERATE_SAVE_COMPLETE);
	RTI_MSG_NAME(NetworkMessage::FEDERATE_SAVE_NOT_COMPLETE);
	RTI_MSG_NAME(NetworkMessage::INITIATE_FEDERATE_SAVE);
	RTI_MSG_NAME(NetworkMessage::FEDERATION_SAVED);
	RTI_MSG_NAME(NetworkMessage::FEDERATION_NOT_SAVED);
	RTI_MSG_NAME(NetworkMessage::REQUEST_FEDERATION_RESTORE);
	RTI_MSG_NAME(NetworkMessage::FEDERATE_RESTORE_COMPLETE);
	RTI_MSG_NAME(NetworkMessage::FEDERATE_RESTORE_NOT_COMPLETE);
	RTI_MSG_NAME(NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED);
	RTI_MSG_NAME(NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED);
	RTI_MSG_NAME(NetworkMessage::FEDERATION_RESTORE_BEGUN);
	RTI_MSG_NAME(NetworkMessage::INITIATE_FEDERATE_RESTORE);
	RTI_MSG_NAME(NetworkMessage::FEDERATION_RESTORED);
	RTI_MSG_NAME(NetworkMessage::FEDERATION_NOT_RESTORED);
	RTI_MSG_NAME(NetworkMessage::PUBLISH_OBJECT_CLASS);
	RTI_MSG_NAME(NetworkMessage::UNPUBLISH_OBJECT_CLASS);
	RTI_MSG_NAME(NetworkMessage::PUBLISH_INTERACTION_CLASS);
	RTI_MSG_NAME(NetworkMessage::UNPUBLISH_INTERACTION_CLASS);
	RTI_MSG_NAME(NetworkMessage::SUBSCRIBE_OBJECT_CLASS);
	RTI_MSG_NAME(NetworkMessage::UNSUBSCRIBE_OBJECT_CLASS);
	RTI_MSG_NAME(NetworkMessage::SUBSCRIBE_INTERACTION_CLASS);
	RTI_MSG_NAME(NetworkMessage::UNSUBSCRIBE_INTERACTION_CLASS);
	RTI_MSG_NAME(NetworkMessage::TURN_INTERACTIONS_ON);
	RTI_MSG_NAME(NetworkMessage::TURN_INTERACTIONS_OFF);
	RTI_MSG_NAME(NetworkMessage::REGISTER_OBJECT);
	RTI_MSG_NAME(NetworkMessage::DISCOVER_OBJECT);
	RTI_MSG_NAME(NetworkMessage::UPDATE_ATTRIBUTE_VALUES);
	RTI_MSG_NAME(NetworkMessage::REFLECT_ATTRIBUTE_VALUES);
	RTI_MSG_NAME(NetworkMessage::SEND_INTERACTION);
	RTI_MSG_NAME(NetworkMessage::RECEIVE_INTERACTION);
	RTI_MSG_NAME(NetworkMessage::DELETE_OBJECT);
	RTI_MSG_NAME(NetworkMessage::REMOVE_OBJECT);
	RTI_MSG_NAME(NetworkMessage::CHANGE_ATTRIBUTE_TRANSPORT_TYPE);
	RTI_MSG_NAME(NetworkMessage::CHANGE_ATTRIBUTE_ORDER_TYPE);
	RTI_MSG_NAME(NetworkMessage::CHANGE_INTERACTION_TRANSPORT_TYPE);
	RTI_MSG_NAME(NetworkMessage::CHANGE_INTERACTION_ORDER_TYPE);
	RTI_MSG_NAME(NetworkMessage::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE);
	RTI_MSG_NAME(NetworkMessage::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE);
	RTI_MSG_NAME(NetworkMessage::IS_ATTRIBUTE_OWNED_BY_FEDERATE);
	RTI_MSG_NAME(NetworkMessage::QUERY_ATTRIBUTE_OWNERSHIP);
	RTI_MSG_NAME(NetworkMessage::ATTRIBUTE_IS_NOT_OWNED);
	RTI_MSG_NAME(NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP);
	RTI_MSG_NAME(NetworkMessage::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
	RTI_MSG_NAME(NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION);
	RTI_MSG_NAME(NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION);
	RTI_MSG_NAME(NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION);
	RTI_MSG_NAME(NetworkMessage::ATTRIBUTE_OWNERSHIP_UNAVAILABLE);
	RTI_MSG_NAME(NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE);
	RTI_MSG_NAME(NetworkMessage::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
	RTI_MSG_NAME(NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION);
	RTI_MSG_NAME(NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE);
	RTI_MSG_NAME(NetworkMessage::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE);
	RTI_MSG_NAME(NetworkMessage::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE);
	RTI_MSG_NAME(NetworkMessage::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION);
	RTI_MSG_NAME(NetworkMessage::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION);
	RTI_MSG_NAME(NetworkMessage::DDM_CREATE_REGION);
	RTI_MSG_NAME(NetworkMessage::DDM_MODIFY_REGION);
	RTI_MSG_NAME(NetworkMessage::DDM_DELETE_REGION);
	RTI_MSG_NAME(NetworkMessage::DDM_ASSOCIATE_REGION);
	RTI_MSG_NAME(NetworkMessage::DDM_UNASSOCIATE_REGION);
	RTI_MSG_NAME(NetworkMessage::DDM_SUBSCRIBE_ATTRIBUTES);
	RTI_MSG_NAME(NetworkMessage::DDM_UNSUBSCRIBE_ATTRIBUTES);
	RTI_MSG_NAME(NetworkMessage::DDM_SUBSCRIBE_INTERACTION);
	RTI_MSG_NAME(NetworkMessage::DDM_UNSUBSCRIBE_INTERACTION);
	RTI_MSG_NAME(NetworkMessage::PROVIDE_ATTRIBUTE_VALUE_UPDATE);
	RTI_MSG_NAME(NetworkMessage::SET_CLASS_RELEVANCE_ADVISORY_SWITCH);
	RTI_MSG_NAME(NetworkMessage::SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH);
	RTI_MSG_NAME(NetworkMessage::SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH);
	RTI_MSG_NAME(NetworkMessage::SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH);
	RTI_MSG_NAME(NetworkMessage::START_REGISTRATION_FOR_OBJECT_CLASS);
	RTI_MSG_NAME(NetworkMessage::STOP_REGISTRATION_FOR_OBJECT_CLASS);

	RTI_MSG_NAME(NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME);
	RTI_MSG_NAME(NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED);
	RTI_MSG_NAME(NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME_FAILED);

        initialized = true ;
    }
}

// ----------------------------------------------------------------------------
//! Increment counter for RTIG message type received.
void
Statistics::rtiService(NetworkMessage::Type service)
{
    rtiServiceSet[service]++ ;
}

// ----------------------------------------------------------------------------
//! Increment counter for Federate message type received.
void
Statistics::federateService(Message::Type service)
{
    federateServiceSet[service]++ ;
}

// ----------------------------------------------------------------------------
//! Display collected data.
ostream &
operator<<(ostream &s, Statistics &stat)
{
    cout << endl << "RTIA: Statistics (processed messages)" << endl ;

    s << "List of federate initiated services " << endl
      << "--------------------------------------------------" << endl ;

    int sentFederateMessages = 0 ;

    for (int i = Message::NOT_USED ; i < Message::LAST ; ++i) {
	int nb = stat.federateServiceSet[i] ;
	if (nb > 0 || stat.displayZero()) {
	    s.width(8);
	    s << nb << ' ' << Statistics::fedMessageName[i] << " (MSG#"
	      << i << ")" << endl ;
        }
        sentFederateMessages += nb ;
    }

    s << endl << "List of RTI initiated services " << endl
      << "--------------------------------------------------" << endl ;

    int sentRtiMessages = 0 ;

    for (int i = NetworkMessage::NOT_USED ; i < NetworkMessage::LAST ; ++i) {
        int nb = stat.rtiServiceSet[i] ;
        if (nb > 0 || stat.displayZero()) {
            s.width(8);
            s << nb << ' ' << Statistics::rtiMessageName[i] << " (MSG#"
	      << i << ")" << endl ;
        }
        sentRtiMessages += nb ;
    }

    s << endl
      << " Number of Federate messages : " << sentFederateMessages << endl
      << " Number of RTIG messages : " << sentRtiMessages << endl ;

    return s ;
}

}} // namespace certi::rtia

#undef FED_MSG_NAME
#undef RTI_MSG_NAME

// $Id: Statistics.cc,v 3.24 2010/08/09 14:51:45 erk Exp $
