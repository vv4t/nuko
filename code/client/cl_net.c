#include "client.h"

#include "../common/net.h"

void client_net_init(client_t *client)
{
  netchan_connect(&client->netchan, "ws://localhost:8000/socket");
}

void client_send_cmd(client_t *client)
{
  netchan_send(&client->netchan, &client->usercmd, sizeof(usercmd_t));
}
