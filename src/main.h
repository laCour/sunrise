/*
 * sunrise: https://git.io/sunrise
 * Copyright (C) 2016  Jonathan la Cour
 *
 * This file is part of sunrise.
 *
 * Sunrise is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sunrise is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sunrise.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAIN_H
#define MAIN_H

#define DBUS_NAMESPACE "me.lacour.Sunrise1"

#define ERR_OOM "Out of memory!\n"
#define ERR_CONN "Connection error (%s)\n"
#define ERR_OWNER "Not primary name owner (%s)\n"
#define ERR_ARGS_EMPTY "Expected arguments!\n"
#define ERR_ARGS_MISSING "Message lacking required argument!\n"
#define ERR_ARG_TYPE_D "Expected argument to be double!\n"

void reply(DBusMessage *msg, DBusConnection *conn);
long sunrise(DBusMessage *msg);
long sunset(DBusMessage *msg);
double brightness(DBusMessage *msg);
void send(DBusConnection *conn, DBusMessage *reply, bool cond);
void sendT(DBusConnection *conn, DBusMessage *reply, DBusMessageIter *args, long a);
void sendD(DBusConnection *conn, DBusMessage *reply, DBusMessageIter *args, double a);
void handleDD(DBusMessage *msg, double *a, double *b);
void introspect(DBusMessage *msg, DBusConnection *conn);
int main(int argc, char **argv);

#endif
