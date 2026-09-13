// The runner includes the actual production Act/PerformAct functions below.
#include <cassert>
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>
#define MAX_STRING_LENGTH 8192
#define LARGE_BUFSIZE 8192
#define TO_CHAR 1
#define TO_VICT 2
#define TO_ROOM 3
#define TO_NOTVICT 4
#define TO_SLEEP 128
#define PLR_WRITING 1
#define AFF_BLIND 2
struct Character;
struct Room { Character *people = nullptr; };
struct Object { Room *in_room = nullptr; };
struct Descriptor {
    std::vector<std::string> output, chat;
    void sendRaw(const char *s) { output.emplace_back(s); }
    void sendWebSocketChat(const char *channel, const std::string &message) { chat.push_back(std::string(channel) + ":" + message); }
};
struct Character {
    const char *name = "someone";
    Descriptor *desc = nullptr;
    Room *in_room = nullptr;
    Character *next_in_room = nullptr;
    bool awake = true, writing = false, blind = false, visible = true, npc = false;
    bool disorientRoll() { return false; }
};
#define PERS(ch,to,...) ((ch)->visible ? (ch)->name : "someone")
#define HMHR(ch) "them"
#define HSHR(ch) "their"
#define HSSH(ch) "they"
#define OBJN(obj,to) "item"
#define OBJS(obj,to) "an item"
#define SANA(obj) "an"
#define PLR_FLAGGED(ch,flag) ((ch)->writing)
#define AFF_FLAGGED(ch,flag) ((ch)->blind)
#define IS_NPC(ch) ((ch)->npc)
#define AWAKE(ch) ((ch)->awake)
#define CAN_SEE(to,ch) ((ch)->visible)
const char *fname(char *s) { return s; }
void Log(const char *, ...) {}
namespace StringUtil {
    const char *cap(char *s) { if (*s) *s = std::toupper(*s); return s; }
}
#include "chat-delivery-impl.inc"
int main() {
    Descriptor senderOutput, receiverOutput, bystanderOutput;
    Room room;
    Character sender{"Speaker", &senderOutput, &room}, receiver{"Listener", &receiverOutput, &room}, bystander{"Bystander", &bystanderOutput, &room};
    room.people = &sender; sender.next_in_room = &receiver; receiver.next_in_room = &bystander;
    Act("$n tells you, 'Scrambled qzx'", 0, &sender, nullptr, &receiver, TO_VICT | TO_SLEEP, nullptr, false, "tell");
    assert(receiverOutput.chat.back() == "tell:" + receiverOutput.output.back());
    assert(senderOutput.chat.empty() && bystanderOutput.chat.empty());
    sender.visible = false;
    Act("$n tells you, 'Hidden name'", 0, &sender, nullptr, &receiver, TO_VICT, nullptr, false, "tell");
    assert(receiverOutput.chat.back() == "tell:Someone tells you, 'Hidden name'\r\n");
    auto count = receiverOutput.chat.size();
    receiver.writing = true;
    Act("$n tells you, 'Blocked'", 0, &sender, nullptr, &receiver, TO_VICT | TO_SLEEP, nullptr, false, "tell");
    assert(receiverOutput.chat.size() == count);
    receiver.writing = false; receiver.awake = false;
    Act("$n whispers to you, 'Asleep'", 0, &sender, nullptr, &receiver, TO_VICT, nullptr, false, "whisper");
    assert(receiverOutput.chat.size() == count);
    Act("$n tells you, 'Wake up'", 0, &sender, nullptr, &receiver, TO_VICT | TO_SLEEP, nullptr, false, "tell");
    assert(receiverOutput.chat.size() == count + 1);
    count = receiverOutput.chat.size(); receiver.awake = true;
    Act("$n attacks!", 0, &sender, nullptr, &receiver, TO_VICT, nullptr, false, nullptr);
    assert(receiverOutput.chat.size() == count); // Ordinary game output stays out of chat.
    Act("$n whispers something to $N.", 0, &sender, nullptr, &receiver, TO_NOTVICT, nullptr, false, "whisper");
    assert(bystanderOutput.chat.back() == "whisper:Someone whispers something to Listener.\r\n");
    assert(bystanderOutput.chat.back().find("Hidden name") == std::string::npos);
    SendChat(&receiver, "say", "%s says '%s'\r\n", "Someone", "50% done");
    assert(receiverOutput.chat.back() == "say:" + receiverOutput.output.back());
    puts("Chat delivery: identical recipient text, hidden names, sleeping/writing filters, whispers, literal formatting, and ordinary output isolation passed.");
}
