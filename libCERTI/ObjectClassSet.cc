// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: ObjectClassSet.cc,v 3.7 2003/01/29 18:32:34 breholee Exp $
// ---------------------------------------------------------------------------

#include "ObjectClassSet.hh"

namespace certi {

static pdCDebug D("OBJECTCLASSSET", "(ObjClSet) - ");

// ---------------------------------------------------------------------------
//! The class is not allocated, only the pointer is memorized.
void
ObjectClassSet::addClass(ObjectClass *newClass)
{
    D.Out(pdInit, "Adding new object class %d.", newClass->Handle);

    newClass->server = server ;

    push_front(newClass);
}

// ---------------------------------------------------------------------------
/*! Build a Parent-Child relation between two object class, by setting the
  Child's Parent handle, and registering the child in the Parent's sonSet.
  Also copy all parent's Attributes in the child Class.
*/
void
ObjectClassSet::buildParentRelation(ObjectClass *child, ObjectClass *parent)
{
    // Register Parent to Son
    child->Father = parent->Handle ;

    // Transfer Security Level
    child->setLevelId(parent->getLevelId());

    // Register Son to Parent
    parent->sonSet.push_front(child->Handle);

    // Copy Parent Attribute into Child class.
    parent->addAttributesToChild(child);
}

// ---------------------------------------------------------------------------
//! Constructor.
ObjectClassSet::ObjectClassSet(SecurityServer *theSecurityServer)
    : list<ObjectClass *>()
{
    // It can be NULL on the RTIA.
    server = theSecurityServer ;
}

// ---------------------------------------------------------------------------
//! Destructor.
ObjectClassSet::~ObjectClassSet(void)
{
    while (!empty()) {
        delete front();
        pop_front();
    }
}

// ---------------------------------------------------------------------------
//! deleteObject.
void
ObjectClassSet::deleteObject(FederateHandle theFederateHandle,
                             ObjectHandle theObjectHandle,
                             const char* theTag)
    throw (DeletePrivilegeNotHeld,
           ObjectNotKnown,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *theClass = getInstanceClass(theObjectHandle);

    D.Out(pdRegister,
          "Federate %d attempts to delete instance %d in class %d.",
          theFederateHandle, theObjectHandle, theClass->Handle);

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = theClass->deleteInstance(theFederateHandle,
                                       theObjectHandle,
                                       theTag);

    // Broadcast RemoveObject message recursively
    ObjectClassHandle currentClass = 0 ;
    if (ocbList != 0) {

        currentClass = theClass->Father ;

        while (currentClass != 0) {
            D.Out(pdRegister,
                  "Broadcasting Remove msg to parent class %d for instance %d.",
                  currentClass, theObjectHandle);

            // It may throw ObjectClassNotDefined
            theClass = getWithHandle(currentClass);
            theClass->broadcastClassMessage(ocbList);

            currentClass = theClass->Father ;
        }

        delete ocbList ;
    }

    D.Out(pdRegister, "Instance %d has been deleted.", theObjectHandle);
}

// ---------------------------------------------------------------------------
//! Print the ObjectClasses tree to the standard output.
void
ObjectClassSet::display(void) const
{
    cout << " ObjectClasses :" << endl ;

    list<ObjectClass *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        (*i)->display();
    }
}

// ---------------------------------------------------------------------------
//! getAttributeHandle.
AttributeHandle
ObjectClassSet::getAttributeHandle(const char *the_name,
                                   ObjectClassHandle the_class) const
    throw (AttributeNotDefined,
           ObjectClassNotDefined,
           RTIinternalError)
{
    ObjectClass *objectClass = NULL ;

    if (the_name == NULL)
        throw RTIinternalError();

    D.Out(pdRequest, "Looking for attribute \"%s\" of class %u...",
          the_name, the_class);

    // It may throw ObjectClassNotDefined.
    objectClass = getWithHandle(the_class);

    return objectClass->getAttributeHandle(the_name);
}

// ---------------------------------------------------------------------------
//! getAttributeName.
const char*
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
    objectClass = getWithHandle(the_class);

    return objectClass->getAttributeName(the_handle);
}

// ---------------------------------------------------------------------------
//! getInstanceClass.
ObjectClass *
ObjectClassSet::getInstanceClass(ObjectHandle theObjectHandle) const
    throw (ObjectNotKnown)
{
    // Try to find Instance's Class
    list<ObjectClass *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if ((*i)->isInstanceInClass(theObjectHandle) == true)
            return (*i);
    }

    D.Out(pdExcept, "Object instance %d not found in any object class.",
          theObjectHandle);
    throw ObjectNotKnown();
}

// ---------------------------------------------------------------------------
//! getObjectClassHandle.
ObjectClassHandle
ObjectClassSet::getObjectClassHandle(const char* the_name) const
    throw (ObjectClassNotDefined,
           RTIinternalError)
{
    if (the_name == NULL)
        throw RTIinternalError();

    D.Out(pdRequest, "Looking for class \"%s\"...", the_name);

    list<ObjectClass *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if (strcmp((*i)->getName(), the_name) == 0)
            return (*i)->Handle ;
    }

    throw ObjectClassNotDefined();
}

// ---------------------------------------------------------------------------
//! getObjectClassName.
const char*
ObjectClassSet::getObjectClassName(ObjectClassHandle the_handle) const
    throw (ObjectClassNotDefined,
           RTIinternalError)
{
    D.Out(pdRequest, "Looking for class %u...", the_handle);

    return getWithHandle(the_handle)->getName();
}

// ---------------------------------------------------------------------------
//! getWithHandle (private method).
ObjectClass *
ObjectClassSet::getWithHandle(ObjectClassHandle theHandle) const
    throw (ObjectClassNotDefined)
{
    list<ObjectClass *>::const_iterator i ;
    for (i = begin(); i != end(); i++) {
        if ((*i)->Handle == theHandle)
            return (*i);
    }

    D.Out(pdExcept, "Unknown Object Class Handle %d .", theHandle);

    throw ObjectClassNotDefined("Unknow class handle.");
}

// ---------------------------------------------------------------------------
//! killFederate.
void ObjectClassSet::killFederate(FederateHandle theFederate)
    throw ()
{
    ObjectClassBroadcastList *ocbList = NULL ;
    ObjectClassHandle currentClass = 0 ;

    list<ObjectClass *>::iterator i ;
    for (i = begin(); i != end(); i++) {
        // Call KillFederate on that class until it returns NULL.
        do {
            D.Out(pdExcept, "Kill Federate Handle %d .", theFederate);
            ocbList = (*i)->killFederate(theFederate);

            D.Out(pdExcept, "Federate Handle %d Killed.", theFederate);

            // Broadcast RemoveObject message recursively
            if (ocbList != 0) {
                currentClass = (*i)->Father ;
                D.Out(pdExcept, "List not NULL");
                while (currentClass != 0) {
                    D.Out(pdRegister,
                          "Broadcasting Remove msg to parent class %d(Killed).",
                          currentClass);

                    // It may throw ObjectClassNotDefined
                    (*i) = getWithHandle(currentClass);

                    (*i)->broadcastClassMessage(ocbList);

                    currentClass = (*i)->Father ;
                }

                delete ocbList ;
            }
        } while (ocbList != NULL);
    }
    D.Out(pdExcept, "End of the KillFederate Procedure.");
}

// ---------------------------------------------------------------------------
//! publish
void
ObjectClassSet::publish(FederateHandle theFederateHandle,
                        ObjectClassHandle theClassHandle,
                        AttributeHandle *theAttributeList,
                        UShort theListSize,
                        bool PubOrUnpub)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           RTIinternalError,
           SecurityError)
{
    // It may throw ObjectClassNotDefined
    ObjectClass *theClass = getWithHandle(theClassHandle);

    if (PubOrUnpub == RTI_TRUE)
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

// ---------------------------------------------------------------------------
//! recursiveDiscovering.
void
ObjectClassSet::recursiveDiscovering(ObjectClassHandle theClassHandle,
                                     FederateHandle theFederate,
                                     ObjectClassHandle theOriginalClass)
{
    // It may throw ObjectClassNotDefined
    ObjectClass *theClass = getWithHandle(theClassHandle);

    D.Out(pdInit, "Recursive Discovering on class %d for Federate %d.",
          theClassHandle, theFederate);

    Boolean result ;
    result = theClass->sendDiscoverMessages(theFederate, theOriginalClass);

    if (result == RTI_TRUE) {
        list<ObjectClassHandle>::const_iterator i = theClass->sonSet.begin();
        for (; i != theClass->sonSet.end(); i++) {
            recursiveDiscovering((*i), theFederate, theOriginalClass);
        }
    }
}

// ---------------------------------------------------------------------------
//! registerInstance.
void
ObjectClassSet::registerInstance(FederateHandle theFederateHandle,
                                 ObjectClassHandle theClassHandle,
                                 ObjectHandle theObjectHandle,
                                 const char* the_object_name)
    throw (InvalidObjectHandle,
           ObjectClassNotDefined,
           ObjectClassNotPublished,
           ObjectAlreadyRegistered,
           RTIinternalError)
{
    ObjectClassHandle currentClass = theClassHandle ;

    D.Out(pdRegister,
          "Federate %d attempts to register instance %d in class %d.",
          theFederateHandle, theObjectHandle, theClassHandle);

    // It may throw ObjectClassNotDefined
    ObjectClass *theClass = getWithHandle(theClassHandle);

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = theClass->registerInstance(theFederateHandle,
                                         theObjectHandle,
                                         the_object_name);

    // Broadcast DiscoverObject message recursively
    if (ocbList != 0) {

        currentClass = theClass->Father ;

        while (currentClass != 0) {
            D.Out(pdRegister,
                  "Broadcasting Discover msg to parent class "
                  "%d for instance %d.",
                  currentClass, theObjectHandle);
            // It may throw ObjectClassNotDefined
            theClass = getWithHandle(currentClass);

            theClass->broadcastClassMessage(ocbList);

            currentClass = theClass->Father ;
        }

        delete ocbList ;
    }

    D.Out(pdRegister, "Instance %d has been registered.", theObjectHandle);
}

// ---------------------------------------------------------------------------
//! subscribe.
void
ObjectClassSet::subscribe(FederateHandle theFederateHandle,
                          ObjectClassHandle theClassHandle,
                          AttributeHandle *theAttributeList,
                          UShort theListSize,
                          bool SubOrUnsub)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           RTIinternalError,
           SecurityError)
{
    // It may throw ObjectClassNotDefined
    ObjectClass *theClass = getWithHandle(theClassHandle);

    if (SubOrUnsub == RTI_TRUE)
        D.Out(pdInit, "Federate %d attempts to subscribe to Object Class %d.",
              theFederateHandle, theClassHandle);
    else
        D.Out(pdTerm, "Federate %d attempts to unsubscribe from Object "
              "Class %d.", theFederateHandle, theClassHandle);

    // It may throw AttributeNotDefined
    Boolean result = theClass->subscribe(theFederateHandle,
                                         theAttributeList,
                                         theListSize,
                                         SubOrUnsub);

    // If Result is true, the Federate has never been a subscriber of this
    // class, so it my have missed some DiscoverObject messages, for this
    // class or one of its sub-class. Therefore, we must start a recursive
    // process to check this class and its subclass for possible instances
    // to discover, until we have reached an already subscribed class in
    // each branch.

    if (result == RTI_TRUE)
        recursiveDiscovering(theClass->Handle, // Start process with this class
                             theFederateHandle, // Send msgs to this Federate
                             theClass->Handle);
    // Discovered objects belong to the same class, the original one.
}

// ---------------------------------------------------------------------------
//! updateAttributeValues.
void
ObjectClassSet::updateAttributeValues(FederateHandle theFederateHandle,
                                      ObjectHandle theObjectHandle,
                                      AttributeHandle *theAttribArray,
                                      AttributeValue *theValueArray,
                                      UShort theArraySize,
                                      FederationTime theTime,
                                      const char* theUserTag)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError,
           InvalidObjectHandle)
{
    // It may throw ObjectNotKnown
    ObjectClass * objectClass = getInstanceClass(theObjectHandle);
    ObjectClassHandle currentClass = objectClass->Handle ;

    D.Out(pdProtocol, "Federate %d Updating object %d from class %d.",
          theFederateHandle, theObjectHandle, currentClass);

    // It may throw a bunch of exceptions
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = objectClass->updateAttributeValues(theFederateHandle,
                                                 theObjectHandle,
                                                 theAttribArray,
                                                 theValueArray,
                                                 theArraySize,
                                                 theTime,
                                                 theUserTag);

    // Broadcast ReflectAttributeValues message recursively
    currentClass = objectClass->Father ;

    while (currentClass != 0) {
        D.Out(pdProtocol,
              "Broadcasting RAV msg to parent class %d for instance %d.",
              currentClass, theObjectHandle);

        // It may throw ObjectClassNotDefined
        objectClass = getWithHandle(currentClass);
        objectClass->broadcastClassMessage(ocbList);

        currentClass = objectClass->Father ;
    }

    delete ocbList ;
}

// ---------------------------------------------------------------------------
//! isAttributeOwnedByFederate.
Boolean
ObjectClassSet::isAttributeOwnedByFederate(ObjectHandle theObject,
                                           AttributeHandle theAttribute,
                                           FederateHandle theFederateHandle)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass * objectClass = getInstanceClass(theObject);

    // It may throw a bunch of exceptions.
    return objectClass->isAttributeOwnedByFederate(theObject,
                                                   theAttribute,
                                                   theFederateHandle);
}

// ---------------------------------------------------------------------------
//! queryAttributeOwnership.
void
ObjectClassSet::queryAttributeOwnership(ObjectHandle theObject,
                                        AttributeHandle theAttribute,
                                        FederateHandle theFederateHandle)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           RTIinternalError)
{
    D.Out(pdDebug, "queryAttributeOwnership sur l'attribut %u de l'objet %u",
          theAttribute, theObject);

    // It may throw ObjectNotKnown
    ObjectClass * objectClass = getInstanceClass(theObject);

    // It may throw a bunch of exceptions.
    objectClass->queryAttributeOwnership(theObject,
                                         theAttribute,
                                         theFederateHandle);
}

// ---------------------------------------------------------------------------
//! negotiatedAttributeOwnershipDivestiture.
void
ObjectClassSet::
negotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                        ObjectHandle theObjectHandle,
                                        AttributeHandle *theAttributeList,
                                        UShort theListSize,
                                        const char *theTag)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeAlreadyBeingDivested,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(theObjectHandle);
    ObjectClassHandle currentClass = objectClass->Handle ;

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = 
        objectClass->negotiatedAttributeOwnershipDivestiture(theFederateHandle,
                                                             theObjectHandle,
                                                             theAttributeList,
                                                             theListSize,
                                                             theTag);

    // Broadcast ReflectAttributeValues message recursively
    currentClass = objectClass->Father ;

    while (currentClass != 0) {
        D.Out(pdProtocol,
              "Broadcasting NAOD msg to parent class %d for instance %d.",
              currentClass, theObjectHandle);

        // It may throw ObjectClassNotDefined
        objectClass = getWithHandle(currentClass);
        objectClass->broadcastClassMessage(ocbList);

        currentClass = objectClass->Father ;
    }

    delete ocbList ;
}

// ---------------------------------------------------------------------------
//! attributeOwnershipAcquisitionIfAvailable.
void
ObjectClassSet::
attributeOwnershipAcquisitionIfAvailable(FederateHandle theFederateHandle,
                                         ObjectHandle theObjectHandle,
                                         AttributeHandle *theAttributeList,
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

// ---------------------------------------------------------------------------
//! unconditionalAttributeOwnershipDivestiture
void
ObjectClassSet::
unconditionalAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                           ObjectHandle theObjectHandle,
                                           AttributeHandle *theAttributeList,
                                           UShort theListSize)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(theObjectHandle);
    ObjectClassHandle currentClass = objectClass->Handle ;

    // It may throw a bunch of exceptions.
    ObjectClassBroadcastList *ocbList = NULL ;
    ocbList = objectClass->
        unconditionalAttributeOwnershipDivestiture(theFederateHandle,
                                                   theObjectHandle,
                                                   theAttributeList,
                                                   theListSize);    

    // Broadcast ReflectAttributeValues message recursively
    currentClass = objectClass->Father ;

    while (currentClass != 0) {
        D.Out(pdProtocol,
              "Broadcasting UAOD msg to parent class %d for instance %d.",
              currentClass, theObjectHandle);

        // It may throw ObjectClassNotDefined
        objectClass = getWithHandle(currentClass);
        objectClass->broadcastClassMessage(ocbList);

        currentClass = objectClass->Father ;
    }

    delete ocbList ;
}

// ---------------------------------------------------------------------------
//! attributeOwnershipAcquisition.
void
ObjectClassSet::
attributeOwnershipAcquisition(FederateHandle theFederateHandle,
                              ObjectHandle theObjectHandle,
                              AttributeHandle *theAttributeList,
                              UShort theListSize,
                              const char *theTag)
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

// ---------------------------------------------------------------------------
//! cancelNegotiatedAttributeOwnershipDivestiture.
void
ObjectClassSet::
cancelNegotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                                              ObjectHandle theObjectHandle,
                                              AttributeHandle *theAttributeList,
                                              UShort theListSize)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeDivestitureWasNotRequested,
           RTIinternalError)
{
    // It may throw ObjectNotKnown
    ObjectClass *objectClass = getInstanceClass(theObjectHandle);

    // It may throw a bunch of exceptions.
    objectClass->
        cancelNegotiatedAttributeOwnershipDivestiture(theFederateHandle,
                                                      theObjectHandle,
                                                      theAttributeList,
                                                      theListSize);
}

// ---------------------------------------------------------------------------
//! attributeOwnershipReleaseResponse.
AttributeHandleSet * ObjectClassSet::
attributeOwnershipReleaseResponse(FederateHandle theFederateHandle,
                                  ObjectHandle theObjectHandle,
                                  AttributeHandle *theAttributeList,
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

// ---------------------------------------------------------------------------
//! cancelAttributeOwnershipAcquisition.
void ObjectClassSet::
cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                    ObjectHandle theObjectHandle,
                                    AttributeHandle *theAttributeList,
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

// $Id: ObjectClassSet.cc,v 3.7 2003/01/29 18:32:34 breholee Exp $
