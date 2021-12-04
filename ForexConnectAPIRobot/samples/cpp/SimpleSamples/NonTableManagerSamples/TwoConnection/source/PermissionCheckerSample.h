#pragma once
class SimpleLog;
class PermissionCheckerSample
{
 public:
    /** Constructor.*/
    PermissionCheckerSample(SimpleLog *log, IO2GSession *session);

    /** Destructor. */
    ~PermissionCheckerSample();

    /** Prints permissions. */
    void printPermissions();
 private:
    /** Prints permission name and status. */
    void print(const char* permissionName, O2GPermissionStatus status);

    SimpleLog *mLog;
    O2G2Ptr<IO2GSession> mSession;
};

