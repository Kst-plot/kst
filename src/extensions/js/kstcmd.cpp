/*
   Copyright (C) 2005 The University of Toronto

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#include <dcopclient.h>
#include <dcopref.h>
#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kglobal.h>
#include <klocale.h>

#include <qstringlist.h>

#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>


#define ERR_NONE		0
#define ERR_NO_SESSION		-1
#define ERR_ATTACHING		-2
#define ERR_KST_TERMINATED	-3
#define ERR_STARTING_SESSION	-4

#define _T(x) i18n(x).local8Bit().data()

QCString kstName;

void ping(int) {
	DCOPClient *client = KApplication::dcopClient();
	if (!client->registeredApplications().contains(kstName)) {
		printf("%s", _T("Kst application process has terminated.\n"));
		rl_cleanup_after_signal();
		exit(ERR_KST_TERMINATED);
	}
}


int run(DCOPRef& ref) {
	DCOPClient *client = KApplication::dcopClient();

	// Disable completion for now.  We might want to add completion of JS
	// keywords and Kst objects at some point.
	rl_bind_key('\t', rl_insert);

	kstName = ref.app();

#ifndef SOLARIS
	signal(SIGALRM, &ping);
	itimerval tv = { { 1, 0 }, { 1, 0 } };
	itimerval old;
	setitimer(ITIMER_REAL, &tv, &old);
#endif

	for (;;) {
		if (!client->registeredApplications().contains(kstName)) {
			printf("%s", _T("Kst application process has terminated.\n"));
			return ERR_KST_TERMINATED;
		}

		char *l = 0L;
		const char *prompt = "kst> ";
		QString line;
		do {
			if (!line.isEmpty()) {
				// replace \ with \n
				line[line.length() - 1] = '\n';
			}

			l = readline(prompt);

			prompt = "";

			if (!l) {
				return 0;
			}
			line += QString(l);
			free(l);
			l = 0L;
		} while (line.endsWith("\\"));

		QString clean = line.stripWhiteSpace();

		if (clean == "exit") {
			return ERR_NONE;
		}

		if (clean == "session") {
			printf("%s\n", ref.app().data());
			continue;
		}

		if (clean == "help") {
			printf("%s", _T("Help:\n"));
			printf("%s", _T("session\t\tDisplay the name of the session in use\n"));
			printf("%s", _T("help\t\tDisplay help\n"));
			printf("%s", _T("exit\t\tExit the command-line interpreter\n"));
			continue;
		}

		if (clean.isEmpty()) {
			continue;
		}

		add_history(line.latin1());

		DCOPReply r = ref.call("evaluate", clean);
		if (r.isValid()) {
			QString res;
			r.get(res);
			if (!res.isEmpty()) {
				printf("%s\n", res.latin1());
			}
		}
	}
}


static KCmdLineOptions options[] = {
  { "+[session]", I18N_NOOP("Kst session to attach to"), 0},
  KCmdLineLastOption
};


int main(int argc, char **argv) {
	KAboutData about("kstcmd", I18N_NOOP("Kst Command Line"), "1.0",
		I18N_NOOP("Kst Command Line"),
		KAboutData::License_GPL,
		I18N_NOOP("(c) 2005-2006 The University of Toronto"), 0,
		"http://kst.kde.org/");

	about.addAuthor("Staikos Computing Services Inc.", I18N_NOOP("Developed for the University of Toronto"), "info@staikos.net");

	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addCmdLineOptions(options);
	KApplication a(false, false);

	QCString session;
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if (args->count() > 0) {
		session = args->arg(0);
	}

	setvbuf(stdout, 0, _IONBF, 0);

	if (session.isEmpty()) {
		DCOPClient *client = KApplication::dcopClient();
		QCStringList apps = client->registeredApplications();
		QCStringList ksts;

		for (QCStringList::ConstIterator i = apps.begin(); i != apps.end(); ++i) {
			if ((*i).left(4) == "kst-") {
				ksts += *i;
			}
		}


		if (ksts.isEmpty()) {
			char *l = readline(_T("Kst does not appear to be running.  Start a new session? "));
			if (!l) {
				return ERR_NONE;
			}

			QString lstr = l;
			free(l);
			l = 0L;
			if (lstr.stripWhiteSpace().lower()[0] != 'y') {
				return ERR_NONE;
			}
			int pid = 0;
			QString err;
			int rc = KApplication::startServiceByDesktopName("kst", QString::null, &err, &session, &pid);
			if (rc != 0) {
				printf(_T("Error starting session: %s\n"), err.latin1());
				return ERR_STARTING_SESSION;
			}

			if (session.isEmpty()) {
				session = QString("kst-%1").arg(pid).latin1();
			}

			time_t startTime = time(NULL);
			printf("%s", _T("Waiting for Kst to start."));
			while (!client->isApplicationRegistered(session)) {
				if (time(NULL) - startTime > 30) {
					printf("%s", _T("\nTimeout waiting for Kst to start\n"));
					return ERR_STARTING_SESSION;
				}
				printf(".");
				sleep(1);
			}
			printf("\n");
		} else if (ksts.count() > 1) {
			QCString parentKst = QCString("kst-") + QString::number(getppid()).latin1();
			if (ksts.contains(parentKst)) {
				session = parentKst;
			} else {
				printf("%s", _T("Please choose a session number to attach to:\n"));
				int j = 0;
				for (QCStringList::ConstIterator i = ksts.begin(); i != ksts.end(); ++i) {
					printf(_T("%2d: %s\n"), ++j, (*i).data());
				}
				printf("> ");
				char choice[10];
				QString l = fgets(choice, 4, stdin);
				l = l.stripWhiteSpace();
				if (l.isEmpty()) {
					printf("%s", _T("No session found.\n"));
					return ERR_NO_SESSION;
				}
				bool ok = false;
				int nchoice = l.toInt(&ok);
				if (ok && nchoice > 0 && nchoice <= j) {
					session = ksts[nchoice - 1];
				} else {
					printf("%s", _T("No session found.\n"));
					return ERR_NO_SESSION;
				}
			}
		} else {
			session = ksts[0];
		}
	}

	DCOPRef ref(session.data(), "KstScript");
	if (ref.isNull()) {
		printf(_T("Error attaching to session %s\n"), session.data());
		return ERR_ATTACHING;
	}

	printf(_T("Attached to Kst session %s\n"), session.data());

	return run(ref);
}

