...
#include <mvas/vas.h>
..
static int create_isolate_vas(const char * vas_name, mode_t permission)
{
  int rc = 0;
  vasid_t vas_id = vas_find(vas_name);

  // If VAS already exists, we want to delete it to have a clean VAS
  if (vas_id >= 0) {
    rc = vas_delete(vas_id);
    if (rc < 0) {
      return -1; // Something wrong
    }
  }

  vas_id = vas_create(vas_name, permission);
  if (vas_id < 0) {
    return -1;
  }

  return vas_id;
}

static int delete_vas(int vas_id)
{
  int rc = 0;

  rc = vas_delete(vas_id);
  if (rc < 0) {
    return -1; // Something wrong
  }
  return rc;
}

static int ap_process_http_async_connection(conn_rec *c)
{
  ...
  AP_DEBUG_ASSERT(cs != NULL);
  AP_DEBUG_ASSERT(cs->state == CONN_STATE_READ_REQUEST_LINE);

  while (cs->state == CONN_STATE_READ_REQUEST_LINE) {
    ap_update_child_status_from_conn(c->sbh, SERVER_BUSY_READ, c);

  if ((r = ap_read_request(c))) {
    // Turn on VAS
    mode_t mode = S_IRWXU | S_IRWXG;
    vasid_t vas_id = create_isolate_vas("VAS_TEST", mode);
    if (vas_id < 0){
      ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(02831)
                    "Error to create isolate vas");
    }
    else {
      rc = vas_attach(0, vas_id, mode);
      if (rc >= 0) {
        rc = vas_switch(vas_id);
        if (rc < 0) {
          ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(02831)
                      "Error to switch vas");
        }
      }
    }
    ... Apache Code ...
    c->keepalive = AP_CONN_UNKNOWN;
    if (r->status == HTTP_OK) {
    ... Apache code ...
    }
    // Turn off VAS
    if (vas_id >= 0) {
      rc = vas_switch(0);
      if (rc < 0) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(02831)
                      "Error to switch back vas");
      }
      rc = vas_detach(0, vas_id);
      if (rc < 0) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(02831)
                      "Error to detach vas");
      }
      rc = vas_delete(vas_id);
      if (rc < 0) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(02831)
                      "Error to delete vas");
      }
    }
    ... HTTPD Code ...

    return OK;
}

static int ap_process_http_sync_connection(conn_rec *c)
{
  // Almost the same of ap_process_http_async_connection
  return OK;
}
