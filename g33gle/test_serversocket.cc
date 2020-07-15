/*
 * Copyright ©2019 Hannah C Tang.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Autumn Quarter 2019 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include "./ServerSocket.h"

#include <iostream>
#include <stdint.h>
#include <string>
#include <unistd.h>

#include "gtest/gtest.h"
#include "./HttpUtils.h"
#include "./ThreadPool.h"
#include "./test_suite.h"

using std::cout;
using std::endl;
using std::string;

namespace hw4 {

static uint16_t portnum;

void TestSSThreadFn(ThreadPool::Task *t) {
  // Create the server socket.
  int listen_fd;
  cout << "Creating ServerSocket on " << portnum << endl;
  ServerSocket ss(portnum);
  cout << "Doing BindAndListen" << endl;
  ASSERT_TRUE(ss.BindAndListen(AF_INET6, &listen_fd));

  // Accept a connection.
  int accept_fd;
  uint16_t cport;
  string caddr, cdns, saddr, sdns;
  cout << "Doing accept..." << endl;
  ASSERT_TRUE(ss.Accept(&accept_fd, &caddr, &cport, &cdns, &saddr, &sdns));

  // It worked!
  cout << "Accept succeeded." << endl;
  close(accept_fd);
  return;
}

TEST(Test_ServerSocket, TestServerSocketBasic) {
  // Create a threadpool, and dispatch a thread to go listen on a
  // server socket on a random port.
  portnum = GetRandPort();
  ThreadPool tp(2);
  ThreadPool::Task tsk(&TestSSThreadFn);
  tp.Dispatch(&tsk);

  // Give the thread a chance to create the socket.
  sleep(1);

  // Connect to the socket
  cout << "Attempting to connect to 127.0.0.1 port "
       << portnum << endl;
  int cfd = -1;
  ASSERT_TRUE(ConnectToServer("127.0.0.1", portnum, &cfd));
  ASSERT_LE(0, cfd);
  HW4Environment::AddPoints(20);
}

}  // namespace hw4
