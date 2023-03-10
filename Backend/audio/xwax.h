/*
 * Copyright (C) 2016 Mark Hills <mark@xwax.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#ifndef XWAX_H
#define XWAX_H

#include "deck.h"

extern char *banner;

#define NOTICE \
  "This software is supplied WITHOUT ANY WARRANTY; without even the implied\n"\
  "warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. This is\n"\
  "free software, and you are welcome to redistribute it under certain\n"\
  "conditions; see the file COPYING for details."

extern size_t ndeck;
extern struct deck deck[];

#endif
