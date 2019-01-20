#ifndef TRSTR_H
#define TRSTR_H

#include <QObject>
#include <string>

inline
std::string trstr(const char *str)
{
  return std::string{QObject::tr(str).toUtf8().data()};
}

#endif // TRSTR_H

