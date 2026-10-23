// Host-side behavioral tests; no physical hardware or ESP32 toolchain involved.
#include <cassert>
#include <iostream>
#include "../firmware/SmartCar/SmartCar.ino"
void tick(uint32_t ms, const std::string &s="") {
 fakeTime+=ms; SerialBT.push(s); loop();
}
void expectMotion(int l,int r){assert(previousLeft==l && previousRight==r);}
void fresh(){
 fakeTime=1000; SerialBT.input.clear(); SerialBT.connected=false;
 connectedBefore=false; validDistance=false; lastSonar=0; lastValidSonar=0;
 fakeEcho=5831; setup(); SerialBT.connected=true; tick(100);
}
int main(){
 fresh(); expectMotion(0,0);
 tick(100,"F"); expectMotion(1,1);
 tick(400,"K"); expectMotion(1,1);
 tick(350,"K"); expectMotion(0,0); // Heartbeat cannot prolong direction.
 tick(1,"B"); expectMotion(-1,-1);
 tick(1,"S"); expectMotion(0,0); assert(mode==Mode::MANUAL);
 tick(100,"A"); expectMotion(1,1); assert(mode==Mode::AUTO);
 fakeEcho=1166; tick(100,"K"); expectMotion(0,0);
 assert(phase==Phase::STOP_BEFORE_REVERSE);
 tick(210,"K"); assert(phase==Phase::REVERSE);
 tick(5,"K"); expectMotion(-1,-1);
 tick(310,"K"); expectMotion(0,0); assert(phase==Phase::STOP_BEFORE_TURN);
 tick(160,"K"); assert(phase==Phase::TURN);
 tick(5,"K"); expectMotion(1,-1);
 tick(460,"K"); expectMotion(0,0); assert(phase==Phase::STOP_AFTER_TURN);
 tick(160,"K"); assert(phase==Phase::CRUISE);
 fakeEcho=5831; tick(100,"K"); expectMotion(1,1);
 tick(1600,"K"); expectMotion(0,0); assert(mode==Mode::MANUAL); // Late K cannot resume.
 tick(100,"A"); expectMotion(1,1);
 fakeEcho=0; tick(100,"K"); expectMotion(0,0); // No echo must not mean clear road.
 tick(1,"M"); tick(100,"F"); expectMotion(0,0);
 fakeEcho=1166; tick(100,"F"); expectMotion(0,0);
 fakeEcho=5831; tick(100,"F"); expectMotion(1,1);
 SerialBT.connected=false; tick(1); expectMotion(0,0);
 SerialBT.connected=true; tick(100); expectMotion(0,0);
 tick(1,"ASF"); expectMotion(0,0); assert(mode==Mode::MANUAL); // S clears pending F.
 tick(100,"A"); tick(200,"?"); tick(1400,"?");
 expectMotion(0,0); assert(mode==Mode::MANUAL);
 fresh(); tick(100,"l"); expectMotion(-1,1);
 tick(100,"R"); expectMotion(1,-1);
 tick(100,"M"); expectMotion(0,0);
 // uint32_t rollover must preserve elapsed-time checks.
 fakeTime=UINT32_MAX-300; lastRx=fakeTime; lastManual=fakeTime;
 manualCommand='B'; tick(100,"K"); expectMotion(-1,-1);
 tick(700,"K"); expectMotion(0,0);
 std::cout << "PASS: startup, commands, watchdogs, auto sequence, sonar failure, disconnect, stop priority, rollover\n";
}
