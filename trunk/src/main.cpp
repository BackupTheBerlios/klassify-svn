#include <kuniqueapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <klocale.h>
#include <dcopclient.h>
#include "klassify.h"

static const char description[] =
    I18N_NOOP("A KDE KPart Application");

static const char version[] = "0.1";

static KCmdLineOptions options[] =
{
    KCmdLineLastOption
};

int main (int argc, char *argv[])
{
	KLocale::setMainCatalogue("kdelibs");
	KAboutData aboutdata("klassify", I18N_NOOP("KDE"),
				version, description,
				KAboutData::License_GPL, "(C) 2005, Dennis Nienhüser");
	aboutdata.addAuthor("Dennis Nienhüser",I18N_NOOP("Developer"),"fragfred@gmx.de");

	KCmdLineArgs::init( argc, argv, &aboutdata );
	KCmdLineArgs::addCmdLineOptions( options );
	KUniqueApplication::addCmdLineOptions();

	if (!KUniqueApplication::start())
	{
		kdDebug() << "klassify is already running!" << endl;
		return (0);
	}

	KUniqueApplication app;
	kdDebug() << "starting klassify " << endl;
	// This app is started automatically, no need for session management
	app.disableSessionManagement();
	/*klassify *service =*/ new klassify;
	kdDebug() << "starting klassify " << endl;
	return app.exec();
}
