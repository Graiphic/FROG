/* SPDX-License-Identifier: Apache-2.0 */
/* A genuine C translation unit, linked to the real frontend library. */
#include <frog/frontend.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEED(condition, message) do { if (!(condition)) { fprintf(stderr, "FRONTEND_C_CONSUMER_FAIL: %s\n", message); return 1; } } while (0)
static frog_frontend_utf8 utf8(const char *value) {
    frog_frontend_utf8 result;result.data=value;result.size=strlen(value);return result;
}
int main(void) {
    const uint32_t version=frog_frontend_api_version();
    frog_frontend_config config={0};frog_frontend_context *context=NULL;frog_frontend_result *result=NULL;
    frog_frontend_result_view view={0};frog_frontend_request request={0};char *descriptor;size_t size;
    NEED(version==FROG_FRONTEND_API_VERSION&&version==3,"The C consumer is not linked to API 3.");
    config.struct_size=(uint32_t)sizeof(config);
    NEED(frog_frontend_context_create(version+1,&config,&context)==FROG_FRONTEND_UNSUPPORTED_API&&context==NULL,"Future API version was accepted.");
    NEED(frog_frontend_context_create(2,&config,&context)==FROG_FRONTEND_UNSUPPORTED_API&&context==NULL,"Historical API 2 was accepted as API 3.");
    config.struct_size=(uint32_t)sizeof(config)-1;
    NEED(frog_frontend_context_create(version,&config,&context)==FROG_FRONTEND_INVALID_ARGUMENT&&context==NULL,"Wrong config size was accepted.");
    config.struct_size=(uint32_t)sizeof(config);
    NEED(frog_frontend_context_create(version,&config,&context)==FROG_FRONTEND_OK&&context!=NULL,"Real frontend context creation failed.");
    request.struct_size=(uint32_t)sizeof(request)-1;
    NEED(frog_frontend_validate(context,&request,&result)==FROG_FRONTEND_INVALID_ARGUMENT&&result==NULL,"Wrong API-3 request size was accepted.");
    request.struct_size=(uint32_t)offsetof(frog_frontend_request,fir_document_id);
    NEED(frog_frontend_validate(context,&request,&result)==FROG_FRONTEND_INVALID_ARGUMENT&&result==NULL,"Historical request prefix was accepted.");
    request.struct_size=(uint32_t)sizeof(request);request.source_bytes=utf8("{");request.source_uri=utf8("memory:c-consumer");
    request.source_document_id=utf8("source:c-consumer");request.source_format_id=utf8("frog.source");
    request.source_format_version=utf8("0.1");request.fir_format_version=utf8("0.1");
    NEED(frog_frontend_validate(context,&request,&result)==FROG_FRONTEND_INVALID_ARGUMENT&&result==NULL,"Validate accepted an absent explicit FIR identity.");
    NEED(frog_frontend_derive(context,&request,&result)==FROG_FRONTEND_INVALID_ARGUMENT&&result==NULL,"Derive inferred FIR identity from Source identity.");
    request.fir_document_id=utf8("fir:c-consumer");
    NEED(frog_frontend_validate(context,&request,&result)==FROG_FRONTEND_OK&&result!=NULL,"Explicit FIR identity did not pass the API metadata boundary.");
    view.struct_size=(uint32_t)sizeof(view);
    NEED(frog_frontend_result_get(result,&view)==FROG_FRONTEND_OK&&view.outcome==FROG_FRONTEND_REJECTED&&!view.fir_json.size&&!view.resolved_source_json.size,"Malformed Source returned executable or resolved artifacts.");
    frog_frontend_result_destroy(result);result=NULL;memset(&view,0,sizeof(view));
    NEED(frog_frontend_describe(context,&result)==FROG_FRONTEND_OK&&result!=NULL,"Real describe failed.");
    view.struct_size=(uint32_t)sizeof(view)-1;
    NEED(frog_frontend_result_get(result,&view)==FROG_FRONTEND_INVALID_ARGUMENT,"Wrong result-view size was accepted.");
    view.struct_size=(uint32_t)offsetof(frog_frontend_result_view,resolved_source_json);
    NEED(frog_frontend_result_get(result,&view)==FROG_FRONTEND_INVALID_ARGUMENT,"Historical result-view prefix was accepted.");
    memset(&view,0,sizeof(view));view.struct_size=(uint32_t)sizeof(view);
    NEED(frog_frontend_result_get(result,&view)==FROG_FRONTEND_OK,"Real result_get failed.");
    NEED(view.operation==FROG_FRONTEND_DESCRIBE&&view.outcome==FROG_FRONTEND_SUCCESS&&!view.fir_json.size&&!view.resolved_source_json.size&&view.descriptor_json.data&&view.descriptor_json.size,"Describe result contains the wrong operation/payload.");
    NEED(view.diagnostics_json.data&&view.diagnostics_json.size&&view.source_maps_json.data&&view.source_maps_json.size&&view.provenance_json.data&&view.provenance_json.size,"Describe omitted complete result documents.");
    size=view.descriptor_json.size;descriptor=(char *)malloc(size);NEED(descriptor!=NULL,"Cannot copy the descriptor for the lifetime check.");memcpy(descriptor,view.descriptor_json.data,size);
    frog_frontend_context_destroy(context);context=NULL;
    memset(&view,0,sizeof(view));view.struct_size=(uint32_t)sizeof(view);
    NEED(frog_frontend_result_get(result,&view)==FROG_FRONTEND_OK&&view.descriptor_json.size==size&&!memcmp(descriptor,view.descriptor_json.data,size),"Result depends on the destroyed context.");
    free(descriptor);frog_frontend_result_destroy(result);frog_frontend_result_destroy(NULL);frog_frontend_context_destroy(NULL);
    printf("FRONTEND_C_CONSUMER_PASS api=%" PRIu32 " descriptor_bytes=%zu invalid_sizes=1 context_independent_result=1\n",version,size);return 0;
}
