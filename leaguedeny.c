#define PURPLE_PLUGINS

#define PLUGIN_VERSION "0.1"
#define PLUGIN_ID "core-leaguedeny"

#include <string.h>
#include <argz.h>

#include <debug.h>
#include <plugin.h>
#include <version.h>
#include <notify.h>
#include <stringref.h>
#include <conversation.h>
#include <prpl.h>
#include <connection.h>

static void send_deny(char * name, PurpleAccount *account) {
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
            xmlnode_free(node);
        }
    }
}

static gboolean message_handler(PurpleAccount *account,
                                char **sender,
                                char **message,
                                PurpleConversation *conv,
                                PurpleMessageFlags *flags) {
    size_t length = strlen(*message) + 1;
    char * text = malloc(length);
    strncpy(text, *message, length);
    argz_replace(&text, &length, "&lt;", "<", NULL);
    argz_replace(&text, &length, "&gt;", ">", NULL);

    xmlnode *node = xmlnode_from_str(text, -1);
    free(text);
    if(node != NULL) {
        if(xmlnode_get_child(node, "inviteId") != NULL) {
            free(*message);
            PurpleStringref * out_msg = purple_stringref_printf("/me has invited you to a League of Legends game.");
            const char * out_str = purple_stringref_value(out_msg);
            length = strlen(out_str) + 1;
            *message = malloc(length);
            strncpy(*message, out_str, length);
            purple_stringref_unref(out_msg);
            send_deny(*sender, account);
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
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

static void init_plugin(PurplePlugin *plugin) {

}

PURPLE_INIT_PLUGIN(leaguedeny, init_plugin, info);
