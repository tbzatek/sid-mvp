/*
 * This file is part of SID.
 *
 * Copyright (C) 2017-2018 Red Hat, Inc. All rights reserved.
 *
 * SID is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * SID is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SID.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ubridge-cmd-module.h"
#include "log.h"

#define ID "md"

static int _md_init(struct sid_module *module, struct sid_ubridge_cmd_mod_context *cmd_mod)
{
	log_debug(ID, "init");
	return 0;
}
SID_UBRIDGE_CMD_MOD_INIT(_md_init)

static int _md_exit(struct sid_module *module, struct sid_ubridge_cmd_mod_context *cmd_mod)
{
	log_debug(ID, "exit");
	return 0;
}
SID_UBRIDGE_CMD_MOD_EXIT(_md_exit)

static int _md_reload(struct sid_module *module, struct sid_ubridge_cmd_mod_context *cmd_mod)
{
	log_debug(ID, "reload");
	return 0;
}
SID_UBRIDGE_CMD_MOD_RELOAD(_md_reload)

static int _md_ident (struct sid_module *module, struct sid_ubridge_cmd_context *cmd)
{
	log_debug(ID, "ident");
	return 0;
}
SID_UBRIDGE_CMD_IDENT(_md_ident)

static int _md_scan_pre(struct sid_module *module, struct sid_ubridge_cmd_context *cmd)
{
	log_debug(ID, "scan-pre");
	return 0;
}
SID_UBRIDGE_CMD_SCAN_PRE(_md_scan_pre)


static int _md_scan_current(struct sid_module *module, struct sid_ubridge_cmd_context *cmd)
{
	log_debug(ID, "scan-current");
	return 0;
}
SID_UBRIDGE_CMD_SCAN_CURRENT(_md_scan_current)

static int _md_scan_next(struct sid_module *module, struct sid_ubridge_cmd_context *cmd)
{
	log_debug(ID, "scan-next");
	return 0;
}
SID_UBRIDGE_CMD_SCAN_NEXT(_md_scan_next)

static int _md_scan_post_current(struct sid_module *module, struct sid_ubridge_cmd_context *cmd)
{
	log_debug(ID, "scan-post-current");
	return 0;
}
SID_UBRIDGE_CMD_SCAN_POST_CURRENT(_md_scan_post_current)

static int _md_scan_post_next(struct sid_module *module, struct sid_ubridge_cmd_context *cmd)
{
	log_debug(ID, "scan-post-next");
	return 0;
}
SID_UBRIDGE_CMD_SCAN_POST_NEXT(_md_scan_post_next)

static int _md_trigger_action_current(struct sid_module *module, struct sid_ubridge_cmd_context *cmd)
{
	log_debug(ID, "trigger-action-current");
	return 0;
}
SID_UBRIDGE_CMD_TRIGGER_ACTION_CURRENT(_md_trigger_action_current)

static int _md_trigger_action_next(struct sid_module *module, struct sid_ubridge_cmd_context *cmd)
{
	log_debug(ID, "trigger-action-next");
	return 0;
}
SID_UBRIDGE_CMD_TRIGGER_ACTION_NEXT(_md_trigger_action_next)

static int _md_error(struct sid_module *module, struct sid_ubridge_cmd_context *cmd)
{
	log_debug(ID, "error");
	return 0;
}
SID_UBRIDGE_CMD_ERROR(_md_error)
