#define PURPLE_PLUGINS

#define PLUGIN_VERSION "0.1"
#define PLUGIN_ID "core-leaguedeny"

#define PREFS_BASE "/plugins/core/leaguedecline"
#define PREF_AUTO_DECLINE PREFS_BASE "/autodecline"
#define PREF_AUTO_REPLY PREFS_BASE "/autoreply"

#include <string.h>

#include <plugin.h>
#include <version.h>
#include <notify.h>
#include <conversation.h>
#include <prpl.h>
#include <connection.h>

#include "strings.h"

static void send_decline(char * name, PurpleAccount *account) {
    PurpleConnection *con = purple_account_get_connection(account);
    if(con) {
        PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(con->prpl);
        if(info && info->send_raw != NULL) {
            xmlnode *node = xmlnode_new("message");
            xmlnode_set_attrib(node, "to",name);
            xmlnode_insert_data(xmlnode_new_child(node, "subject"), "GAME_INVITE_REJECT", -1);
            int length;
            char * out_str = xmlnode_to_str(node, &length);
            info->send_raw(con, out_str, length);
            free(out_str);
            xmlnode_free(node);
        }
    }
}

static void send_response(PurpleConversation *conv) {
    const char * msg = purple_prefs_get_string(PREF_AUTO_REPLY);
    if(msg != NULL && strlen(msg) != 0) {
        PurpleConvIm *im = purple_conversation_get_im_data(conv);
        purple_conv_im_send_with_flags(im, msg, PURPLE_MESSAGE_AUTO_RESP);
    }
}

static gboolean message_handler(PurpleAccount *account,
                                char **sender,
                                char **message,
                                PurpleConversation *conv,
                                PurpleMessageFlags *flags) {
    char *text = str_replace(*message, "&lt;", "<", false);
    text = str_replace(text, "&gt;", ">", true);

    xmlnode *node = xmlnode_from_str(text, -1);
    free(text);
    if(node != NULL) {
        if(xmlnode_get_child(node, "inviteId") != NULL) {
            free(*message);
            *message = g_strdup_printf("/me has invited you to a League of Legends game.");
            if(purple_prefs_get_bool(PREF_AUTO_DECLINE)) send_decline(*sender, account);
            send_response(conv);
        }
    }
    xmlnode_free(node);
    return FALSE;
}

static gboolean plugin_load(PurplePlugin *plugin) {
    purple_debug_info(PLUGIN_ID, "Starting plugin");

    purple_signal_connect(purple_conversations_get_handle(),
                          "receiving-im-msg", plugin,
                          PURPLE_CALLBACK(message_handler), 0);
    return TRUE;
}

static PurplePluginPrefFrame *get_plugin_pref_frame(PurplePlugin *plugin) {
    PurplePluginPrefFrame *frame;
    PurplePluginPref *pref;

    frame = purple_plugin_pref_frame_new();
    pref = purple_plugin_pref_new_with_name(PREF_AUTO_DECLINE);
    purple_plugin_pref_set_label(pref, "Automatically decline invites");
    purple_plugin_pref_frame_add(frame, pref);
    pref = purple_plugin_pref_new_with_name(PREF_AUTO_REPLY);
    purple_plugin_pref_set_label(pref, "Automated response to invites");
    purple_plugin_pref_frame_add(frame, pref);

    return frame;
}

static PurplePluginUiInfo prefs_info = {
    get_plugin_pref_frame,
    0,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

static PurplePluginInfo info = {
    PURPLE_PLUGIN_MAGIC,
    PURPLE_MAJOR_VERSION,
    PURPLE_MINOR_VERSION,
    PURPLE_PLUGIN_STANDARD,
    NULL,
    0,
    NULL,
    PURPLE_PRIORITY_DEFAULT,

    PLUGIN_ID,
    "League Decline",
    PLUGIN_VERSION,

    "Declines League invites",
    "Recognizes League of Legends invite messages, displays them in a useful matter and sends a decline message",
    0,
    "http://syntax-austria.org",

    plugin_load,
    NULL,
    NULL,

    NULL,
    NULL,
    &prefs_info,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

static void init_plugin(PurplePlugin *plugin) {
    purple_prefs_add_none(PREFS_BASE);
    purple_prefs_add_bool(PREF_AUTO_DECLINE, TRUE);
    purple_prefs_add_string(PREF_AUTO_REPLY, NULL);
}

PURPLE_INIT_PLUGIN(leaguedeny, init_plugin, info);
