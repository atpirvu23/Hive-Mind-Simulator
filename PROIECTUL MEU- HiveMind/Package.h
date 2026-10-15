#pragma once
#include "Utils.h"
// cream aceasta structura pentru a stii unde trebuie sa ajunga pachetul, cati bani valoreaza
// si cat timp mai are

struct Package
{
  int id;
  Position destination;
  int reward;
  int deadline;
  int createdAt;
  bool assigned;
  bool delivered;
  bool expired;
  int deliveredAt;

  // constructor simplu
  Package(int _id, Position _dest, int _rew, int _deadline, int _tick)
      : id(_id), destination(_dest), reward(_rew),
        deadline(_deadline), createdAt(_tick), assigned(false),
        delivered(false), expired(false), deliveredAt(-1) {}
};