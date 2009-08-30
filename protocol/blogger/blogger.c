/* generated by gxr: '../../util/gxr/gxr-gen.rb blogger blogger.gxr'. */

#include "gxr-internal.h"
#include "blogger.h"


const char *
blogger_newPost(GXRContext *ctx, const char * arg_appkey, const char * arg_journal, const char * arg_user, const char * arg_password, const char * arg_content, gboolean arg_publish, GError **err) {
	xmlDocPtr doc; xmlNodePtr np;
	char *retval = NULL;
	const char * ret = 0; /* meaningless unless err isn't set. */

	gxr_make_doc(&doc, &np, "blogger.newPost");
	gxr_add_param_string (np, arg_appkey);
	gxr_add_param_string (np, arg_journal);
	gxr_add_param_string (np, arg_user);
	gxr_add_param_string (np, arg_password);
	gxr_add_param_string (np, arg_content);
	gxr_add_param_boolean(np, arg_publish);

	if (!gxr_run_request(ctx, doc, GXR_VALUE_STRING, &retval, err))
		goto out;

	ret = g_strdup(retval);
	xmlFree(retval);
out:
	xmlFreeDoc(doc);
	return ret;
}

gboolean
blogger_deletePost(GXRContext *ctx, const char * arg_appkey, const char * arg_postId, const char * arg_user, const char * arg_password, gboolean arg_publish, GError **err) {
	xmlDocPtr doc; xmlNodePtr np;
	char *retval = NULL;
	gboolean ret = 0; /* meaningless unless err isn't set. */

	gxr_make_doc(&doc, &np, "blogger.deletePost");
	gxr_add_param_string (np, arg_appkey);
	gxr_add_param_string (np, arg_postId);
	gxr_add_param_string (np, arg_user);
	gxr_add_param_string (np, arg_password);
	gxr_add_param_boolean(np, arg_publish);

	if (!gxr_run_request(ctx, doc, GXR_VALUE_BOOLEAN, &retval, err))
		goto out;


	xmlFree(retval);
out:
	xmlFreeDoc(doc);
	return ret;
}

int
blogger_editPost(GXRContext *ctx, const char * arg_appkey, const char * arg_postId, const char * arg_user, const char * arg_password, const char * arg_content, gboolean arg_publish, GError **err) {
	xmlDocPtr doc; xmlNodePtr np;
	char *retval = NULL;
	int ret = 0; /* meaningless unless err isn't set. */

	gxr_make_doc(&doc, &np, "blogger.editPost");
	gxr_add_param_string (np, arg_appkey);
	gxr_add_param_string (np, arg_postId);
	gxr_add_param_string (np, arg_user);
	gxr_add_param_string (np, arg_password);
	gxr_add_param_string (np, arg_content);
	gxr_add_param_boolean(np, arg_publish);

	if (!gxr_run_request(ctx, doc, GXR_VALUE_INT, &retval, err))
		goto out;

	ret = atoi(retval);
	xmlFree(retval);
out:
	xmlFreeDoc(doc);
	return ret;
}

