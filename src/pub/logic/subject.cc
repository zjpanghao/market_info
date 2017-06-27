//
// Created by Harvey on 2017/1/11.
//

#include "subject.h"

#include "observer.h"

namespace strade_logic {

Subject::Subject() {

}

Subject::~Subject() {

}

void Subject::Attach(Observer* observer) {
  this->m_lst.push_back(observer);
}

void Subject::Detach(Observer* observer) {
  std::list<Observer*>::iterator iter;
  iter = find(m_lst.begin(), m_lst.end(), observer);
  if (iter != m_lst.end()) {
    m_lst.erase(iter);
  }
}

void Subject::Notify(int opcode) {
  std::list<Observer*>::iterator iter = this->m_lst.begin();
  for (; iter != m_lst.end(); ) {
    if ((*iter)->stale()) {
      m_lst.erase(iter++);
      continue;
    }
    (*iter++)->Update(opcode);
  }
}

} /* namespace strade_logic */
