// **********************************************************************
//
// Copyright (c) 2001
// MutableRealms, Inc.
// Huntsville, AL, USA
//
// All Rights Reserved
//
// **********************************************************************

#include <Freeze/Application.h>
#include <IceStorm/TopicManagerI.h>
#include <IceStorm/TraceLevels.h>

using namespace std;
using namespace Ice;
using namespace IceStorm;

namespace IceStorm
{

class Server : public Freeze::Application
{
public:

    Server(const string& dbEnvName) :
	Freeze::Application(dbEnvName)
    {
    }

    void usage();
    virtual int runFreeze(int, char*[], const Freeze::DBEnvironmentPtr&);
};

} // End namespace IceStorm

int
main(int argc, char* argv[])
{
    //
    // TODO: Hardcoded directory must be removed.
    //
    Server app("db");
    return app.main(argc, argv);
}

void
IceStorm::Server::usage()
{
    cerr << "Usage: " << appName() << " [options]\n";
    cerr <<	
	"Options:\n"
	"-h, --help           Show this message.\n"
	"-v, --version        Display the Ice version.\n"
	;
}

int
IceStorm::Server::runFreeze(int argc, char* argv[], const Freeze::DBEnvironmentPtr& dbEnv)
{
    for (int i = 1; i < argc; ++i)
    {
	if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
	{
	    usage();
	    return EXIT_SUCCESS;
	}
	else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
	{
	    cout << ICE_STRING_VERSION << endl;
	    return EXIT_SUCCESS;
	}
	else
	{
	    cerr << appName() << ": unknown option `" << argv[i] << "'" << endl;
	    usage();
	    return EXIT_FAILURE;
	}
    }

    Freeze::DBPtr dbTopicManager = dbEnv->openDB("topicmanager");

    //PropertiesPtr properties = communicator()->getProperties();

    TraceLevelsPtr traceLevels = new TraceLevels(communicator()->getProperties());
    ObjectAdapterPtr adapter = communicator()->createObjectAdapter("TopicManager");
    ObjectPtr object = new TopicManagerI(communicator(), adapter, traceLevels, dbTopicManager);
    adapter->add(object, "TopicManager");
    adapter->activate();

    shutdownOnInterrupt();
    communicator()->waitForShutdown();
    ignoreInterrupt();

    // TODO: topic manager ::reap()

    return EXIT_SUCCESS;
}
