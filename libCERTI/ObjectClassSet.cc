// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: ObjectClassSet.cc,v 3.49 2009/11/21 21:18:28 erk Exp $
// ----------------------------------------------------------------------------

// Project
#include "Object.hh"
#include "ObjectClass.hh"
#include "ObjectClassSet.hh"
#include "ObjectClassBroadcastList.hh"
#include "SecurityServer.hh"
#include "PrettyDebug.hh"
#include "Named.hh"

// Standard
#include <iosfwd>
#include <sstream>

using std::endl ;

namespace certi {

static PrettyDebug D("OBJECTCLASSSET", __FILE__);
static PrettyDebug G("GENDOC",__FILE__) ;

ObjectClassSet::ObjectClassSet(SecurityServer *theSecurityServer, bool isRootClassSet)
 : TreeNamedAndHandledSet<ObjectClass>("Object Classes",isRootClassSet)
{
    // It can be NULL on the RTIA.
    server = theSecurityServer ;
}

// ----------------------------------------------------------------------------
//! Destructor.
ObjectClassSet::~ObjectClassSet()
{

} /* end of ~ObjectClassSet */

void
ObjectClassSet::addClass(ObjectClass *newClass,ObjectClass *parentClass) throw (RTIinternalError)
{
	D.Out(pdInit, "Adding new object class %d.", newClass->getHandle());
	/* link to server */
    newClass->server = server ;
	add(newClass,parentClass);

} /* end of addClass */

// ----------------------------------------------------------------------------
//! deleteObject with time.
void
ObjectClassSet::deleteObject(FederateHandle federate,
                             ObjectHandle object,
			     FederationTime theTime,
                             const std::string& tag)
    throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *oclass = getInstanceClass(object);

    D.Out(pdRegister,
          "Federate %d attempts to delete instance %d in class %d.",
          federate, object, oclass->getHandle());

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = oclass->deleteInstance(federate,
                                                               object,
							       theTime,
                                                               tag);

    // Broadcast RemoveObject message recursively
    ObjectClassHandle current_class = 0 ;
    if (ocbList != 0) {

        current_class = oclass->getSuperclass();

        while (current_class) {
            D.Out(pdRegister,
                  "Broadcasting Remove msg to parent class %d for instance %d.",
                  current_class, object);

            // It may throw ObjectClassNotDefined
            oclass = getObjectFromHandle(current_class);
            oclass->broadcastClassMessage(ocbList);

            current_class = oclass->getSuperclass();
        }

        delete ocbList ;
    }
}
// ----------------------------------------------------------------------------
//! deleteObject without time.
void
ObjectClassSet::deleteObject(FederateHandle federate,
                             ObjectHandle object,
                             const std::string& tag)
    throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *oclass = getInstanceClass(object);

    D.Out(pdRegister,
          "Federate %d attempts to delete instance %d in class %d.",
          federate, object, oclass->getHandle());

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = oclass->deleteInstance(federate,
                                                               object,
                                                               tag);

    // Broadcast RemoveObject message recursively
    ObjectClassHandle current_class = 0 ;
    if (ocbList != 0) {

        current_class = oclass->getSuperclass();

        while (current_class) {
            D.Out(pdRegister,
                  "Broadcasting Remove msg to parent class %d for instance %d.",
                  current_class, object);

            // It may throw ObjectClassNotDefined
            oclass = getObjectFromHandle(current_class);
            oclass->broadcastClassMessage(ocbList);

            current_class = oclass->getSuperclass();
        }

        delete ocbList ;
    }

    D.Out(pdRegister, "Instance %d has been deleted.", object);
}

// ----------------------------------------------------------------------------
//! getAttributeHandle.
AttributeHandle
ObjectClassSet::getAttributeHandle(const std::string& the_name,
                                   ObjectClassHandle the_class) const
    throw (NameNotFound, ObjectClassNotDefined, RTIinternalError)
{
    G.Out(pdGendoc,"enter ObjectClassSet::getAttributeHandle");

    ObjectClass *objectClass = 0 ;
    AttributeHandle handle ;

    if (the_name.empty())
        throw RTIinternalError("name is null");

    D.Out(pdRequest, "Looking for attribute \"%s\" of class %u...",
          the_name.c_str(), the_class);

    // It may throw ObjectClassNotDefined.
    objectClass = getObjectFromHandle(the_class);


    try
        {
         handle = objectClass->getAttributeHandle(the_name);
         G.Out(pdGendoc,"exit ObjectClassSet::getAttributeHandle");
         return handle ;
         }
    catch ( NameNotFound )
         {
         G.Out(pdGendoc,"exit  ObjectClassset::getAttributeHandle on NameNotFound");
         throw NameNotFound (the_name) ;
         }
}

// ----------------------------------------------------------------------------
//! getAttributeName.
const std::string&
ObjectClassSet::getAttributeName(AttributeHandle the_handle,
                                 ObjectClassHandle the_class) const
    throw (AttributeNotDefined,
           ObjectClassNotDefined,
           RTIinternalError)
{
    ObjectClass *objectClass = NULL ;

    D.Out(pdRequest, "Looking for attribute %u of class %u...",
          the_handle, the_class);

    // It may throw ObjectClassNotDefined.
    objectClass = getObjectFromHandle(the_class);

    return objectClass->getAttributeName(the_handle);
}

// ----------------------------------------------------------------------------
//! getInstanceClass.
ObjectClass *
ObjectClassSet::getInstanceClass(ObjectHandle theObjectHandle) const
    throw (ObjectNotKnown)
{
    handled_const_iterator i ;
    for (i = fromHandle.begin(); i != fromHandle.end(); ++i) {
        if (i->second->isInstanceInClass(theObjectHandle) == true)
            return (i->second);
    }

    std::stringstream msg;
    msg << "ObjectHandle <" << theObjectHandle <<"> not found in any object class.";
    D.Out(pdExcept, msg.str().c_str());
    throw ObjectNotKnown(msg.str());
}

// ----------------------------------------------------------------------------
/** Get object
 */
Object *
ObjectClassSet::getObject(ObjectHandle h) const
    throw (ObjectNotKnown)
{
	handled_const_iterator i ;

	for (i = fromHandle.begin(); i != fromHandle.end(); ++i) {
                if (!i->second->isInstanceInClass(h)) {
                        continue;
                }
                return i->second->getInstanceWithID(h);
	}
	throw ObjectNotKnown("");
}

// ----------------------------------------------------------------------------
//! getObjectClassHandle.
ObjectClassHandle
ObjectClassSet::getObjectClassHandle(const std::string& class_name) const
throw (NameNotFound){
	return getHandleFromName(class_name);
} /* end of getObjectClassHandle */

// ----------------------------------------------------------------------------
//! getObjectClassName.
const std::string&
ObjectClassSet::getObjectClassName(ObjectClassHandle the_handle) const
    throw (ObjectClassNotDefined)
{
    D.Out(pdRequest, "Looking for class %u...", the_handle);
    return getNameFromHandle(the_handle);
}

// ----------------------------------------------------------------------------
//! killFederate.
void ObjectClassSet::killFederate(FederateHandle theFederate)
    throw ()
{
    ObjectClassBroadcastList *ocbList      = NULL ;
    ObjectClassHandle         currentClass = 0 ;

    handled_iterator i;

    for (i = fromHandle.begin(); i != fromHandle.end(); ++i) {
        // Call KillFederate on that class until it returns NULL.
        do {
            D.Out(pdExcept, "Kill Federate Handle %d .", theFederate);
            ocbList = i->second->killFederate(theFederate);

            D.Out(pdExcept, "Federate Handle %d Killed.", theFederate);

            // Broadcast RemoveObject message recursively
            if (ocbList != 0) {
                currentClass = i->second->getSuperclass();
                D.Out(pdExcept, "List not NULL");
                while (currentClass != 0) {
                    D.Out(pdRegister,
                          "Broadcasting Remove msg to parent class %d(Killed).",
                          currentClass);

                    // It may throw ObjectClassNotDefined
                    i->second = getObjectFromHandle(currentClass);

                    i->second->broadcastClassMessage(ocbList);

                    currentClass = i->second->getSuperclass();
                }

                delete ocbList ;
            }
        } while (ocbList != NULL);
    }
    D.Out(pdExcept, "End of the KillFederate Procedure.");
} /* end of killFederate */

// ----------------------------------------------------------------------------
//! publish
void
ObjectClassSet::publish(FederateHandle theFederateHandle,
                        ObjectClassHandle theClassHandle,
                        std::vector <AttributeHandle> &theAttributeList,
                        UShort theListSize,
                        bool PubOrUnpub)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           RTIinternalError,
           SecurityError)
{
    // It may throw ObjectClassNotDefined
    ObjectClass *theClass = getObjectFromHandle(theClassHandle);

    if (PubOrUnpub)
        D.Out(pdInit, "Federate %d attempts to publish Object Class %d.",
              theFederateHandle, theClassHandle);
    else
        D.Out(pdTerm, "Federate %d attempts to unpublish Object Class %d.",
              theFederateHandle, theClassHandle);

    // It may throw AttributeNotDefined
    theClass->publish(theFederateHandle,
                      theAttributeList,
                      theListSize,
                      PubOrUnpub);
}

// ----------------------------------------------------------------------------
//! registerInstance.
void
ObjectClassSet::registerObjectInstance(FederateHandle the_federate,
                                       Object *the_object,
                                       ObjectClassHandle the_class)
    throw (InvalidObjectHandle,
           ObjectClassNotDefined,
           ObjectClassNotPublished,
           ObjectAlreadyRegistered,
           RTIinternalError)
{
    ObjectClassHandle currentClass = the_class ;

    // It may throw ObjectClassNotDefined
    ObjectClass *theClass = getObjectFromHandle(the_class);

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = theClass->registerObjectInstance(the_federate, the_object,
                                               the_class);

    // Broadcast DiscoverObject message recursively
    if (ocbList != 0) {

        currentClass = theClass->getSuperclass();

        while (currentClass != 0) {
            D.Out(pdRegister,
                  "Broadcasting Discover msg to parent class "
                  "%d for instance %d.",
                  currentClass, the_object);
            // It may throw ObjectClassNotDefined
            theClass = getObjectFromHandle(currentClass);

            theClass->broadcastClassMessage(ocbList);

            currentClass = theClass->getSuperclass();
        }

        delete ocbList ;
    }

    Debug(D, pdRegister) << "Instance " << the_object << " has been registered."
                  << endl ;
}

// ----------------------------------------------------------------------------
/** Subscribes a federate to a set of attributes with a region. Sends the
    discovery messages if necessary.
    @param federate Federate to subscribe
    @param class_handle Class to be subscribed
    @param attributes List of attributes to be subscribed
    @param nb Number of attributes
    @param region Subscription region (0 for default)
 */
void
ObjectClassSet::subscribe(FederateHandle federate,
                          ObjectClassHandle class_handle,
                          std::vector <AttributeHandle> &attributes,
                          int nb,
			  const RTIRegion *region)
    throw (ObjectClassNotDefined, AttributeNotDefined, RTIinternalError,
           SecurityError)
{
    ObjectClass *object_class = getObjectFromHandle(class_handle);

    bool need_discover = object_class->subscribe(federate, attributes, nb, region);

    if (need_discover)
	object_class->recursiveDiscovering(federate, class_handle);
}

// ----------------------------------------------------------------------------
//! updateAttributeValues with time
void
ObjectClassSet::updateAttributeValues(FederateHandle federate,
                                      ObjectHandle object_handle,
                                      std::vector <AttributeHandle> &attributes,
                                      std::vector <AttributeValue_t> &values,
                                      UShort nb,
                                      FederationTime time,
                                      const std::string& tag)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError,
           InvalidObjectHandle)
{
    Object *object = getObject(object_handle);
    ObjectClass *object_class = getObjectFromHandle(object->getClass());
    ObjectClassHandle current_class = object_class->getHandle();

    D.Out(pdProtocol, "Federate %d Updating object %d from class %d.",
          federate, object_handle, current_class);

    // It may throw a bunch of exceptions
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = object_class->updateAttributeValues(
	federate, object, attributes, values, nb, time, tag);

    // Broadcast ReflectAttributeValues message recursively
    current_class = object_class->getSuperclass();

    while (current_class != 0) {
        D.Out(pdProtocol,
              "Broadcasting RAV msg to parent class %d for instance %d.",
              current_class, object_handle);

        // It may throw ObjectClassNotDefined
        object_class = getObjectFromHandle(current_class);
        object_class->broadcastClassMessage(ocbList, object);

        current_class = object_class->getSuperclass();
    }

    delete ocbList ;
}

// ----------------------------------------------------------------------------
//! updateAttributeValues without time
void
ObjectClassSet::updateAttributeValues(FederateHandle federate,
                                      ObjectHandle object_handle,
                                      std::vector <AttributeHandle> &attributes,
                                      std::vector <AttributeValue_t> &values,
                                      UShort nb,
                                      const std::string& tag)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError,
           InvalidObjectHandle)
{
    Object *object = getObject(object_handle);
    ObjectClass *object_class = getObjectFromHandle(object->getClass());
    ObjectClassHandle current_class = object_class->getHandle();

    D.Out(pdProtocol, "Federate %d Updating object %d from class %d.",
          federate, object_handle, current_class);

    // It may throw a bunch of exceptions
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = object_class->updateAttributeValues(
	federate, object, attributes, values, nb, tag);

    // Broadcast ReflectAttributeValues message recursively
    current_class = object_class->getSuperclass();

    while (current_class != 0) {
        D.Out(pdProtocol,
              "Broadcasting RAV msg to parent class %d for instance %d.",
              current_class, object_handle);

        // It may throw ObjectClassNotDefined
        object_class = getObjectFromHandle(current_class);
        object_class->broadcastClassMessage(ocbList, object);

        current_class = object_class->getSuperclass();
    }

    delete ocbList ;
}

// ----------------------------------------------------------------------------
//! negotiatedAttributeOwnershipDivestiture.
void
ObjectClassSet::
negotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                        ObjectHandle theObjectHandle,
                                        std::vector <AttributeHandle> &theAttributeList,
                                        UShort theListSize,
                                        const std::string& theTag)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeAlreadyBeingDivested,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(theObjectHandle);
    ObjectClassHandle currentClass = objectClass->getHandle();

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList =
        objectClass->negotiatedAttributeOwnershipDivestiture(theFederateHandle,
                                                             theObjectHandle,
                                                             theAttributeList,
                                                             theListSize,
                                                             theTag);

    // Broadcast ReflectAttributeValues message recursively
    currentClass = objectClass->getSuperclass();

    while (currentClass != 0) {
        D.Out(pdProtocol,
              "Broadcasting NAOD msg to parent class %d for instance %d.",
              currentClass, theObjectHandle);

        // It may throw ObjectClassNotDefined
        objectClass = getObjectFromHandle(currentClass);
        objectClass->broadcastClassMessage(ocbList);

        currentClass = objectClass->getSuperclass();
    }

    delete ocbList ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisitionIfAvailable.
void
ObjectClassSet::
attributeOwnershipAcquisitionIfAvailable(FederateHandle theFederateHandle,
                                         ObjectHandle theObjectHandle,
                                         std::vector <AttributeHandle> &theAttributeList,
                                         UShort theListSize)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           AttributeAlreadyBeingAcquired,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass * objectClass = getInstanceClass(theObjectHandle);

    // It may throw a bunch of exceptions.
    objectClass->attributeOwnershipAcquisitionIfAvailable(theFederateHandle,
                                                          theObjectHandle,
                                                          theAttributeList,
                                                          theListSize);
}

// ----------------------------------------------------------------------------
//! unconditionalAttributeOwnershipDivestiture
void
ObjectClassSet::
unconditionalAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                           ObjectHandle theObjectHandle,
                                           std::vector <AttributeHandle> &theAttributeList,
                                           UShort theListSize)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(theObjectHandle);
    ObjectClassHandle currentClass = objectClass->getHandle();

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = objectClass->
        unconditionalAttributeOwnershipDivestiture(theFederateHandle,
                                                   theObjectHandle,
                                                   theAttributeList,
                                                   theListSize);

    // Broadcast ReflectAttributeValues message recursively
    currentClass = objectClass->getSuperclass();

    while (currentClass != 0) {
        D.Out(pdProtocol,
              "Broadcasting UAOD msg to parent class %d for instance %d.",
              currentClass, theObjectHandle);

        // It may throw ObjectClassNotDefined
        objectClass = getObjectFromHandle(currentClass);
        objectClass->broadcastClassMessage(ocbList);

        currentClass = objectClass->getSuperclass();
    }

    delete ocbList ;
}

// ----------------------------------------------------------------------------
//! attributeOwnershipAcquisition.
void
ObjectClassSet::
attributeOwnershipAcquisition(FederateHandle theFederateHandle,
                              ObjectHandle theObjectHandle,
                              std::vector <AttributeHandle> &theAttributeList,
                              UShort theListSize,
                              const std::string& theTag)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass * objectClass = getInstanceClass(theObjectHandle);

    // It may throw a bunch of exceptions.
    objectClass->attributeOwnershipAcquisition(theFederateHandle,
                                               theObjectHandle,
                                               theAttributeList,
                                               theListSize,
                                               theTag);
}

// ----------------------------------------------------------------------------
//! attributeOwnershipReleaseResponse.
AttributeHandleSet * ObjectClassSet::
attributeOwnershipReleaseResponse(FederateHandle theFederateHandle,
                                  ObjectHandle theObjectHandle,
                                  std::vector <AttributeHandle> &theAttributeList,
                                  UShort theListSize)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateWasNotAskedToReleaseAttribute,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(theObjectHandle);

    // It may throw a bunch of exceptions.
    return(objectClass->attributeOwnershipReleaseResponse(theFederateHandle,
                                                          theObjectHandle,
                                                          theAttributeList,
                                                          theListSize));
}

// ----------------------------------------------------------------------------
//! cancelAttributeOwnershipAcquisition.
void ObjectClassSet::
cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                    ObjectHandle theObjectHandle,
                                    std::vector <AttributeHandle> &theAttributeList,
                                    UShort theListSize)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(theObjectHandle);

    // It may throw a bunch of exceptions.
    objectClass->cancelAttributeOwnershipAcquisition(theFederateHandle,
                                                     theObjectHandle,
                                                     theAttributeList,
                                                     theListSize);
}

} // namespace certi

// $Id: ObjectClassSet.cc,v 3.49 2009/11/21 21:18:28 erk Exp $
