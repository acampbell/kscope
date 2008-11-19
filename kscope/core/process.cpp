/***************************************************************************
 *   Copyright (C) 2007-2008 by Elad Lahav
 *   elad_lahav@users.sourceforge.net
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#include "process.h"

namespace KScope
{

namespace Core
{

Process::Process(QObject* parent) : QProcess(parent)
{
	connect(this, SIGNAL(readyReadStandardOutput()), this,
	        SLOT(readStandardOutput()));
	connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this,
	        SLOT(handleFinished(int, QProcess::ExitStatus)));
	connect(this, SIGNAL(error(QProcess::ProcessError)), this,
			SLOT(handleError(QProcess::ProcessError)));
	connect(this, SIGNAL(stateChanged(QProcess::ProcessState)), this,
	        SLOT(handleStateChange(QProcess::ProcessState)));
}

Process::~Process()
{
}

void Process::setDeleteOnExit()
{
	deleteOnExit_ = true;
}

void Process::readStandardOutput()
{
	QStringList lines;
	QStringList::Iterator itr;

	// Read from standard output.
	stdOut_ += readAllStandardOutput();

	// Split the standard output into lines.
	// If the last line dos not end with a newline character, do not parse it.
	// Instead, keep it in the stored buffer and wait for more input.
	lines = stdOut_.split('\n', QString::SkipEmptyParts);
	if (stdOut_.endsWith('\n'))
		stdOut_ = "";
	else
		stdOut_ = lines.takeLast();

	// Parse each of the complete lines received.
	for(itr = lines.begin(); itr != lines.end(); ++itr) {
		if (!step(*itr)) {
			emit parseError();
			return;
		}
	}
}

void Process::readStandardError()
{
#ifndef QT_NO_DEBUG
	qDebug() << readAllStandardError();
#endif // !QT_NO_DEBUG
}

void Process::handleFinished(int code, QProcess::ExitStatus status)
{
#ifndef QT_NO_DEBUG
	qDebug() << "Process finished" << code << status;
#else
	(void)code;
	(void)status;
#endif // !QT_NO_DEBUG
}

void Process::handleError(QProcess::ProcessError code)
{
#ifndef QT_NO_DEBUG
	qDebug() << "Process error" << code;
#else
	(void)code;
#endif // !QT_NO_DEBUG
}

void Process::handleStateChange(QProcess::ProcessState state)
{
#ifndef QT_NO_DEBUG
	qDebug() << "Process state" << state;
#endif // !QT_NO_DEBUG

	if (state == QProcess::NotRunning && deleteOnExit_)
		deleteLater();
}

}

}
