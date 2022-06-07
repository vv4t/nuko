#include "sv_local.h"

entity_t sv_new_client(sock_t sock)
{
  entity_t entity = edict_add_entity(&sv.edict, SV_ES_CLIENT); // Createa new entity
  
  if (entity == -1) {
    net_sock_disconnect(sock);
    
    log_printf(LOG_WARNING, "too many clients.");
    return -1;
  }
  
  sv.bg.transform[entity]             = (bg_transform_t) {0};
  sv.bg.motion[entity]                = (bg_motion_t) {0};
  
  sv.bg.transform[entity].position.y  = 10;
  
  sv.bg.capsule[entity].radius        = 0.5f;
  sv.bg.capsule[entity].height        = 1.0f;
  
  sv.bg.health[entity].max            = 100;
  sv.bg.health[entity].now            = sv.bg.health[entity].max;
  
  sv.bg.model[entity]                 = BG_MDL_SKULL;
  
  sv.bg.attack[entity].active         = false;
  sv.bg.attack[entity].next_attack    = 0;
  
  sv.score[entity].kills              = 0;
  sv.score[entity].deaths             = 0;
  
  sv.respawn[entity].spawn_time       = 0;
  sv.respawn[entity].invul_time       = 0;
  sv.respawn[entity].alive            = true;
  
  sv.client[entity].sock              = sock;
  sv.client[entity].cmd_tail          = 0;
  sv.client[entity].cmd_head          = 0;
  sv.client[entity].snapshot_ack      = 0;
  snprintf(sv.client[entity].name, sizeof(sv.client[entity].name), "GUEST%i", (rand() % 100000) + 100000);
  
  static char msg[128];
  if (snprintf(msg, sizeof(msg), "[SERVER] '%s' joined the game", sv.client[entity].name) < sizeof(msg))
    sv_send_chat(msg);
  
  sv.num_clients++;
  
  return entity;
}

void sv_client_parse_frame(entity_t entity, const frame_t *frame)
{
  switch (frame->netcmd) {
  case NETCMD_CHAT:
    sv_client_parse_chat(entity, frame);
    break;
  case NETCMD_NAME:
    sv_client_parse_name(entity, frame);
    break;
  case NETCMD_SCORE:
    sv_client_parse_score(entity, frame);
    break;
  case NETCMD_USERCMD:
    sv_client_parse_usercmd(entity, frame);
    break;
  case NETCMD_CLIENT_INFO:
    break;
  case NETCMD_SNAPSHOT:
    break;
  }
}

void sv_client_parse_score(entity_t entity, const frame_t *frame)
{
  static char msg[sizeof(frame->data.chat.content)];
  
  if (sv_print_score(msg, sizeof(msg)))
    sv_client_send_chat(entity, msg);
}

void sv_client_parse_name(entity_t entity, const frame_t *frame)
{
  if (strnlen(frame->data.name.name, sizeof(frame->data.name.name)) >= sizeof(frame->data.name.name)) {
    sv_client_send_chat(entity, "[SERVER] name is too long.");
    return;
  }
  
  static char msg[128];
  if (snprintf(msg, sizeof(msg), "[SERVER] '%s' changed their name to '%s'", sv.client[entity].name, frame->data.name.name) < sizeof(msg))
    sv_send_chat(msg);
  
  strncpy(sv.client[entity].name, frame->data.name.name, sizeof(sv.client[entity].name));
}

void sv_client_parse_chat(entity_t entity, const frame_t *frame)
{
  static char msg[128];
  if (snprintf(msg, sizeof(msg), "[CHAT] %s: %s", sv.client[entity].name, frame->data.chat.content) < sizeof(msg))
    sv_send_chat(msg);
}

void sv_client_parse_usercmd(entity_t entity, const frame_t *frame)
{
  sv_client_t *svc = &sv.client[entity];
  
  svc->cmd_queue[svc->cmd_head++ % MAX_CMD_QUEUE] = frame->data.usercmd.d;
  if (svc->cmd_head - svc->cmd_tail >= MAX_CMD_QUEUE)
    svc->cmd_tail = svc->cmd_head - MAX_CMD_QUEUE + 1;
  
  svc->snapshot_ack = frame->data.usercmd.ack;
}

void sv_client_send_client_info(entity_t entity)
{
  frame_t frame;
  frame.netcmd = NETCMD_CLIENT_INFO;
  frame.data.client_info.entity = entity;
  
  strcpy(frame.data.client_info.map_name, sv.map_name);
  
  frame_send(sv.client[entity].sock, &frame);
}

void sv_client_send_chat(entity_t entity, const char *text)
{
  frame_t frame;
  frame.netcmd = NETCMD_CHAT;
  strncpy(frame.data.chat.content, text, sizeof(frame.data.chat.content));
  frame_send(sv.client[entity].sock, &frame);
}

void sv_client_disconnect(entity_t entity)
{
  static char msg[128];
  int str_len = snprintf(msg, sizeof(msg), "[SERVER] '%s' disconnected", sv.client[entity].name);
  
  net_sock_disconnect(sv.client[entity].sock);
  edict_remove_entity(&sv.edict, entity);
  sv.num_clients--;
  
  if (str_len < sizeof(msg))
    sv_send_chat(msg);
}
