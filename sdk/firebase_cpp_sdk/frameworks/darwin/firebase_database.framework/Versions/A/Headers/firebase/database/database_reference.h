// Copyright 2016 Google Inc. All Rights Reserved.

#ifndef FIREBASE_DATABASE_CLIENT_CPP_SRC_INCLUDE_FIREBASE_DATABASE_DATABASE_REFERENCE_H_
#define FIREBASE_DATABASE_CLIENT_CPP_SRC_INCLUDE_FIREBASE_DATABASE_DATABASE_REFERENCE_H_

#include <map>
#include <string>
#include "firebase/database/disconnection.h"
#include "firebase/database/query.h"
#include "firebase/database/transaction.h"
#include "firebase/future.h"
#include "firebase/internal/common.h"
#include "firebase/variant.h"

namespace firebase {
namespace database {
namespace internal {
class DatabaseInternal;
class DatabaseReferenceInternal;
}  // namespace internal

class DataSnapshot;

/// DatabaseReference represents a particular location in your Database and can
/// be used for reading or writing data to that Database location.
///
/// This class is the starting point for all Database operations. After you've
/// initialized it with a URL, you can use it to read data, write data, and to
/// create new DatabaseReference instances.
class DatabaseReference : public Query {
 public:
  /// @brief Required virtual destructor.
  virtual ~DatabaseReference();

  /// @brief Copy constructor. It's totally okay (and efficient) to copy
  /// DatabaseReference instances, as they simply point to the same location in
  /// the database.
  ///
  /// @param[in] reference DatabaseReference to copy from.
  DatabaseReference(const DatabaseReference& reference);

  /// @brief Copy assignment operator. It's totally okay (and efficient) to copy
  /// DatabaseReference instances, as they simply point to the same location in
  /// the database.
  ///
  /// @param[in] reference DatabaseReference to copy from.
  ///
  /// @returns Reference to the destination DatabaseReference.
  DatabaseReference& operator=(const DatabaseReference& reference);

#if defined(FIREBASE_USE_MOVE_OPERATORS) || defined(DOXYGEN)
  /// @brief Move constructor. Moving is an efficient operation for
  /// DatabaseReference instances.
  ///
  /// @param[in] reference DatabaseReference to move data from.
  DatabaseReference(DatabaseReference&& reference);

  /// @brief Move assignment operator. Moving is an efficient operation for
  /// DatabaseReference instances.
  ///
  /// @param[in] reference DatabaseReference to move data from.
  ///
  /// @returns Reference to the destination DatabaseReference.
  DatabaseReference& operator=(DatabaseReference&& reference);
#endif  // defined(FIREBASE_USE_MOVE_OPERATORS) || defined(DOXYGEN)

  /// @brief Gets the database to which we refer.
  ///
  /// The pointer will remain valid indefinitely.
  ///
  /// @returns Firebase Database instance that this DatabaseReference refers to.
  Database* database() const;

  /// @brief Gets the database to which we refer.
  ///
  /// The pointer will remain valid indefinitely.
  ///
  /// @returns Firebase Database instance that this DatabaseReference refers to.
  ///
  /// @deprecated Renamed to database().
  FIREBASE_DEPRECATED Database* GetDatabase() const { return database(); }

  /// @brief Gets the string key of this database location.
  ///
  /// The pointer is only valid while the DatabaseReference remains in memory.
  ///
  /// @returns String key of this database location, which will remain valid in
  /// memory until the DatabaseReference itself goes away.
  const char* key() const;

  /// @brief Gets the string key of this database location.
  ///
  /// The pointer is only valid while the DatabaseReference remains in memory.
  ///
  /// @returns String key of this database location, which will remain valid in
  /// memory until the DatabaseReference itself goes away.
  FIREBASE_DEPRECATED const char* GetKey() const { return key(); }

  /// @brief Gets the string key of this database location.
  ///
  /// @returns String key of this database location.
  std::string key_string() const;

  /// @brief Gets the string key of this database location.
  ///
  /// @returns String key of this database location.
  ///
  /// @deprecated Renamed to key_string().
  FIREBASE_DEPRECATED std::string GetKeyString() const { return key_string(); }

  /// @brief Returns true if this reference refers to the root of the database.
  ///
  /// @returns true if this reference refers to the root of the database, false
  /// otherwise.
  bool is_root() const;

  /// @brief Returns true if this reference refers to the root of the database.
  ///
  /// @returns true if this reference refers to the root of the database, false
  /// otherwise.
  ///
  /// @deprecated Renamed to is_root().
  FIREBASE_DEPRECATED bool IsRoot() const { return is_root(); }

  /// @brief Returns true if this reference is valid, false if it is not
  /// valid. An invalid reference could be returned by Database::GetReference()
  /// or Database::GetReferenceFromUrl() if you specify an incorrect location,
  /// or calling Query::GetReference() on an invalid query.
  ///
  /// @returns true if this reference is valid, false if this reference is
  /// invalid.
  virtual bool is_valid() const;

  /// @brief Returns true if this reference is valid, false if it is not
  /// valid. An invalid reference could be returned by Database::GetReference()
  /// or Database::GetReferenceFromUrl() if you specify an incorrect location,
  /// or calling Query::GetReference() on an invalid query.
  ///
  /// @returns true if this reference is valid, false if this reference is
  /// invalid.
  ///
  /// @deprecated Renamed to is_valid().
  FIREBASE_DEPRECATED virtual bool IsValid() const { return is_valid(); }

  /// @brief Gets the parent of this location, or get this location again if
  /// IsRoot().
  ///
  /// @returns Parent of this location in the database, unless this location is
  /// the root, in which case it returns this same location again.
  DatabaseReference GetParent() const;

  /// @brief Gets the root of the database.
  ///
  /// @returns Root of the database.
  DatabaseReference GetRoot() const;

  /// @brief Gets a reference to a location relative to this one.
  ///
  /// @param[in] path Path relative to this snapshot's location.
  /// The pointer only needs to be valid during this call.
  ///
  /// @returns Child relative to this location.
  DatabaseReference Child(const char* path) const;

  /// @brief Gets a reference to a location relative to this one.
  ///
  /// @param[in] path Path relative to this snapshot's location.
  ///
  /// @returns Child relative to this location.
  DatabaseReference Child(const std::string& path) const;

  /// @brief Automatically generates a child location, create a reference to it,
  /// and returns that reference to it.
  ///
  /// @returns A newly created child, with a unique key.
  DatabaseReference PushChild() const;

  /// @brief Removes the value at this location from the database.
  ///
  /// This is an asynchronous operation which takes time to execute, and uses
  /// firebase::Future to return its result.
  ///
  /// @returns A Future result, which will complete when the operation either
  /// succeeds or fails. When the Future is completed, if its Error is
  /// kErrorNone, the operation succeeded.
  ///
  /// @note Only one RemoveValue() should be running on a given database
  /// location at the same time. If you need to run multiple operations at once,
  /// use RunTransaction().
  Future<void> RemoveValue();

  /// @brief Gets the result of the most recent call to RemoveValue();
  ///
  /// @returns Result of the most recent call to RemoveValue().
  Future<void> RemoveValueLastResult();

  /// @brief Run a user-supplied callback function, possibly multiple times, to
  /// perform an atomic transaction on the database.
  ///
  /// @see firebase::database::DoTransaction() for more information.
  ///
  /// param[in] transaction_function The user-supplied function that will be
  /// called, possibly multiple times, to perform the database transaction.
  /// param[in] fire_local_events If true, events will be triggered for
  /// intermediate state changes during the transaction. If false, only the
  /// final state will cause events to be triggered.
  ///
  /// @returns A Future result, which will complete when the transaction either
  /// succeeds or fails. When the Future is completed, if its Error is
  /// kErrorNone, the operation succeeded and the transaction was committed, and
  /// the new value of the data will be returned in the DataSnapshot result. If
  /// the Error is kErrorTransactionAbortedByUser, the transaction was aborted
  /// because the transaction function returned kTransactionResultAbort, and the
  /// old value will be returned in DataSnapshot. Otherwise, if some other error
  /// occurred, Error and ErrorMessage will be set and DataSnapshot will be
  /// invalid.
  ///
  /// @note Only one RunTransaction() should be running on a given database
  /// location at the same time.
  Future<DataSnapshot> RunTransaction(DoTransaction* transaction_function,
                                      bool trigger_local_events = true);

  /// @brief Get the result of the most recent call to RunTransaction().
  ///
  /// @returns Results of the most recent call to RunTransaction().
  Future<DataSnapshot> RunTransactionLastResult();

  /// @brief Sets the priority of this field, which controls its sort
  /// order relative to its siblings.
  ///
  /// In Firebase, children are sorted in the following order:
  /// 1. First, children with no priority.
  /// 2. Then, children with numerical priority, sorted numerically in
  ///    ascending order.
  /// 3. Then, remaining children, sorted lexicographically in ascending order
  ///    of their text priority.
  ///
  /// Children with the same priority (including no priority) are sorted by
  /// key:
  /// A. First, children with keys that can be parsed as 32-bit integers,
  ///    sorted in ascending numerical order of their keys.
  /// B. Then, remaining children, sorted in ascending lexicographical order
  ///    of their keys.
  ///
  /// This is an asynchronous operation which takes time to execute, and uses
  /// firebase::Future to return its result.
  ///
  /// @param[in] priority Sort priority for this child relative to its siblings.
  /// The Variant types accepted are Null, Int64, Double, and String. Other
  /// types will return kErrorInvalidVariantType.
  ///
  /// @returns A Future result, which will complete when the operation either
  /// succeeds or fails. When the Future is completed, if its Error is
  /// kErrorNone, the operation succeeded.
  ///
  /// @note Only one SetPriority() should be running on a given database
  /// location
  /// at the same time. If you need to run multiple operations at once, use
  /// RunTransaction().
  Future<void> SetPriority(Variant priority);

  /// @brief Gets the result of the most recent call to SetPriority().
  ///
  /// @returns Result of the most recent call to SetPriority().
  Future<void> SetPriorityLastResult();

  /// @brief Sets the data at this location to the given value.
  ///
  /// This is an asynchronous operation which takes time to execute, and uses
  /// firebase::Future to return its result.
  ///
  /// @param[in] value The value to set this location to. The Variant's type
  /// corresponds to the types accepted by the database JSON:
  /// Null: Deletes this location from the database.
  /// Int64: Inserts an integer value into this location.
  /// Double: Inserts a floating point value into this location.
  /// String: Inserts a string into this location.
  ///         (Accepts both Mutable and Static strings)
  /// Vector: Inserts a JSON array into this location. The elements can be any
  ///         Variant type, including Vector and Map.
  /// Map: Inserts a JSON associative array into this location. The keys must
  ///      be of type String (or Int64/Double which are converted to String).
  ///      The values can be any Variant type, including Vector and Map.
  ///
  /// @returns A Future result, which will complete when the operation either
  /// succeeds or fails. When the Future is completed, if its Error is
  /// kErrorNone, the operation succeeded.
  ///
  /// @note Only one SetValue() should be running on a given database location
  /// at the same time. If you need to run multiple operations at once, use
  /// RunTransaction().
  Future<void> SetValue(Variant value);

  /// @brief Gets the result of the most recent call to SetValue().
  ///
  /// @returns Result of the most recent call to SetValue().
  Future<void> SetValueLastResult();

  /// @brief Sets both the data and priority of this location. See SetValue()
  /// and SetPriority() for context on the parameters.
  ///
  /// This is an asynchronous operation which takes time to execute, and uses
  /// firebase::Future to return its result.
  ///
  /// @param[in] value The value to set this location to. See SetValue() for
  /// information on the types accepted.
  /// @param[in] priority The priority to set this location to. See
  /// SetPriority() for information on the types accepted.
  ///
  /// @returns A Future result, which will complete when the operation either
  /// succeeds or fails. When the Future is completed, if its Error is
  /// kErrorNone, the operation succeeded.
  ///
  /// @note Only one SetValueAndPriority() should be running on a given database
  /// location at the same time. SetValueAndPriority() can't be used on the same
  /// location at the same time as either SetValue() or SetPriority(), and will
  /// return kErrorConflictingOperationInProgress if you try. If you need to run
  /// multiple operations at once, use RunTransaction().
  Future<void> SetValueAndPriority(Variant value, Variant priority);

  /// @brief Get the result of the most recent call to SetValueAndPriority().
  ///
  /// @returns Result of the most recent call to SetValueAndPriority().
  Future<void> SetValueAndPriorityLastResult();

  /// @brief Updates the specified child keys to the given values.
  ///
  /// @param[in] values A variant of type Map. The keys are the paths to update
  /// and must be of type String (or Int64/Double which are converted to
  /// String). The values can be any Variant type. A value of Variant type Null
  /// will delete the child.
  ///
  /// @returns A Future result, which will complete when the operation either
  /// succeeds or fails. When the Future is completed, if its Error is
  /// kErrorNone, the operation succeeded.
  ///
  /// @note This method will return kErrorConflictingOperationInProgress if it
  /// is run at the same time as SetValue(), SetValueAndPriority(), or
  /// RemoveValue() in the same location.
  Future<void> UpdateChildren(Variant values);

  /// @brief Updates the specified child keys to the given values.
  ///
  /// This is an asynchronous operation which takes time to execute, and uses
  /// firebase::Future to return its result.
  ///
  /// @param[in] values The paths to update, and their new child values. A value
  /// of type Null will delete that particular child.
  ///
  /// @returns A Future result, which will complete when the operation either
  /// succeeds or fails. When the Future is completed, if its Error is
  /// kErrorNone, the operation succeeded.
  inline Future<void> UpdateChildren(
      const std::map<std::string, Variant>& values) {
    return UpdateChildren(Variant(values));
  }

  /// @brief Gets the result of the most recent call to either version of
  /// UpdateChildren().
  ///
  /// @returns Result of the most recent call to UpdateChildren().
  Future<void> UpdateChildrenLastResult();

  /// @brief Get the absolute URL of this reference.
  ///
  /// @returns The absolute URL of the location this reference refers to.
  std::string url() const;

  /// @brief Get the absolute URL of this reference.
  ///
  /// @returns The absolute URL of the location this reference refers to.
  ///
  /// @deprecated Renamed to url().
  FIREBASE_DEPRECATED std::string GetUrl() const { return url(); }

  /// @brief Get the disconnect handler, which controls what actions the server
  /// will perform to this location's data when this client disconnects.
  ///
  /// @returns Disconnection handler for this location. You can use this to
  /// queue up operations on the server to be performed when the client
  /// disconnects.
  DisconnectionHandler* OnDisconnect();

  /// @brief Manually disconnect Firebase Realtime Database from the server, and
  /// disable automatic reconnection. This will affect all other instances of
  /// DatabaseReference as well.
  void GoOffline();

  /// @brief Manually reestablish connection to the Firebase Realtime Database
  /// server and enable automatic reconnection. This will affect all other
  /// instances of DatabaseReference as well.
  void GoOnline();

 protected:
  /// @cond FIREBASE_APP_INTERNAL
  explicit DatabaseReference(internal::DatabaseReferenceInternal* internal);
  /// @endcond

 private:
  /// @cond FIREBASE_APP_INTERNAL
  friend class DataSnapshot;
  friend class Query;
  friend class internal::DatabaseInternal;
  friend bool operator==(const DatabaseReference& lhs,
                         const DatabaseReference& rhs);
  /// @endcond

  internal::DatabaseReferenceInternal* internal_;
};

/// @brief Compares two DatabaseReference instances.
///
/// @param[in] lhs A DatabaseReference.
/// @param[in] rhs A DatabaseReference to compare against.
///
/// @returns True if the DatabaseReference instances have the same URL. False
/// otherwise.
bool operator==(const DatabaseReference& lhs, const DatabaseReference& rhs);

}  // namespace database
}  // namespace firebase

#endif  // FIREBASE_DATABASE_CLIENT_CPP_SRC_INCLUDE_FIREBASE_DATABASE_DATABASE_REFERENCE_H_