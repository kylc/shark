#ifndef REMOTE_H_
#define REMOTE_H_

#define MSG_SYNC_STR ("abcd")

struct remote_msg_t {
  float speed; // 0..1
  float angle; // -90..90
};

#endif
