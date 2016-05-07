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

#include <dbus/dbus.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "sun.h"

extern char _binary_src_interface_xml_start;
extern char _binary_src_interface_xml_end;

void reply(DBusMessage *msg, DBusConnection *conn) {
  DBusMessage *reply;
  DBusMessageIter args;

  long t_response = -1;
  double d_response = -1;

  if (dbus_message_is_method_call(msg, DBUS_NAMESPACE, "Sunrise")) { t_response = sunrise(msg); }
  if (dbus_message_is_method_call(msg, DBUS_NAMESPACE, "Sunset")) { t_response = sunset(msg); }
  if (dbus_message_is_method_call(msg, DBUS_NAMESPACE, "Brightness")) { d_response = brightness(msg); }

  reply = dbus_message_new_method_return(msg);
  dbus_message_iter_init_append(reply, &args);

  if (t_response >= 0) { sendT(conn, reply, &args, t_response); }
  if (d_response >= 0) { sendD(conn, reply, &args, d_response); }

  dbus_connection_flush(conn);
  dbus_message_unref(reply);
}

long sunrise(DBusMessage *msg) {
  double lat, lng;
  handleDD(msg, &lat, &lng);
  return Sun(lat, lng).sunrise();
}

long sunset(DBusMessage *msg) {
  double lat, lng;
  handleDD(msg, &lat, &lng);
  return Sun(lat, lng).sunset();
}

double brightness(DBusMessage *msg) {
  double lat, lng;
  handleDD(msg, &lat, &lng);
  return Sun(lat, lng).brightness();
}

void send(DBusConnection *conn, DBusMessage *reply, bool cond) {
  if (!cond) {
    fprintf(stderr, ERR_OOM);
    exit(1);
  }

  if (!dbus_connection_send(conn, reply, 0)) {
    fprintf(stderr, ERR_OOM);
    exit(1);
  }
}

void sendT(DBusConnection *conn, DBusMessage *reply, DBusMessageIter *args, long a) {
  send(conn, reply, dbus_message_iter_append_basic(args, DBUS_TYPE_UINT64, &a));
}

void sendD(DBusConnection *conn, DBusMessage *reply, DBusMessageIter *args, double a) {
  send(conn, reply, dbus_message_iter_append_basic(args, DBUS_TYPE_DOUBLE, &a));
}

void handleDD(DBusMessage *msg, double *a, double *b) {
  DBusMessageIter args;

  if (!dbus_message_iter_init(msg, &args)) {
    fprintf(stderr, ERR_ARGS_EMPTY);
  } else if (DBUS_TYPE_DOUBLE != dbus_message_iter_get_arg_type(&args)) {
    fprintf(stderr, ERR_ARG_TYPE_D);
  }

  dbus_message_iter_get_basic(&args, a);


  if (!dbus_message_iter_next(&args)) {
    fprintf(stderr, ERR_ARGS_MISSING);
  } else if (DBUS_TYPE_DOUBLE != dbus_message_iter_get_arg_type(&args)) {
    fprintf(stderr, ERR_ARG_TYPE_D);
  }

  dbus_message_iter_get_basic(&args, b);
}

void introspect(DBusMessage *msg, DBusConnection *conn) {
  DBusMessage *reply;

  if (dbus_message_is_method_call(msg, "org.freedesktop.DBus.Introspectable", "Introspect")) {
    char *introspect_xml = &_binary_src_interface_xml_start;

    reply = dbus_message_new_method_return(msg);

    send(conn, reply, dbus_message_append_args(reply, DBUS_TYPE_STRING, &introspect_xml, DBUS_TYPE_INVALID));

    dbus_connection_flush(conn);
    dbus_message_unref(reply);
  }
}

int main(int argc, char **argv) {
  DBusMessage *msg;
  DBusConnection *conn;
  DBusError err;

  dbus_error_init(&err);

  conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if (dbus_error_is_set(&err)) {
    fprintf(stderr, ERR_CONN, err.message);
    dbus_error_free(&err);
  }

  if (conn == NULL) { exit(1); }

  int ret = dbus_bus_request_name(conn, DBUS_NAMESPACE, DBUS_NAME_FLAG_REPLACE_EXISTING, &err);

  if (dbus_error_is_set(&err)) {
    fprintf(stderr, "%s\n", err.message);
    dbus_error_free(&err);
  }

  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
    fprintf(stderr, ERR_OWNER, ret);
    exit(1);
  }

  while (true) {
    dbus_connection_read_write(conn, 0);
    msg = dbus_connection_pop_message(conn);

    if (msg == NULL) {
      sleep(1);
      continue;
    }

    introspect(msg, conn);
    reply(msg, conn);
    dbus_message_unref(msg);
  }

  return 0;
}
