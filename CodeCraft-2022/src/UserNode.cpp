#include "UserNode.h"

void UserNode::init(string un){
    this->un = un;
}
void UserNode::set_dBW_at_t(int ti, int dBW){
    this->dBWList[ti] = dBW;
}

