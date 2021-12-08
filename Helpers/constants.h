#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <vector>

#include <QString>
#include <QFont>
#include <QTranslator>

using std::vector;

namespace Helpers {

// application
const QString appName = "File Finder v 1.0";
const int appWidthDefault = 1300;
const int appHeightDefault = 800;

// root path search
const QString emptyRootPathLinux = "";
const QString emptyRootPathWin = "C:/";

const QString catalogLabel = "Current catalog: ";
const QString buttonLabelToStart = "START";
const QString buttonLabelToCancel = "CANCEL";
const QString placeholderFiles = "*.*";
const QString defaultSearchPuttern = "*.*";
const QString toolTipForPattern = "use ? for one character\n* for multiple characters";

const int listModelSize = 50;

const QString saveButtonLabel = "Save...";
const int saveButtonMinWidth = 300;

const QString noFilesInSearchResult = "No files.";
const QString foundFilesInSearchResult = "Found %1 file(s):";
const QString searchingInSearchResult = "Searching...";

const int layoutProportions[] = {2, 3};

const int catalogWidgetMargins[] = {10, 10, 0, 10};
const int controlWidgetMargins[] = {0, 0, 0, 0};
}
#endif // CONSTANTS_H
