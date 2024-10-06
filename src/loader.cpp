#include "../headers/loader.h"

#include "../libraries/json.hpp"
using json = nlohmann::json;

#include "../headers/card_text.h"

Card* loadCard(std::string jason)
{
        std::ifstream f(jason);
        json data = json::parse(f);
        Card* result;
        if (data.find("text") != data.end())
        {
            std::string text = data["text"];
            result = new Card(data["name"],"./sprites/cardfaces/" + std::string(data["sprite"]) + ".png",text);
        }
        else if (data.find("stats") != data.end())
        {
            ResourceStats stats;
            stats.food = (data["stats"].find("food") != data["stats"].end()) ? (int)data["stats"]["food"] : 0;
            stats.coins = (data["stats"].find("gold") != data["stats"].end()) ? (int)data["stats"]["gold"] : 0;
            stats.damage = (data["stats"].find("damage") != data["stats"].end()) ? (int)data["stats"]["damage"] : 0;
            result = new Card(data["name"],"./sprites/cardfaces/" + std::string(data["sprite"]) + ".png",stats);

        }
        f.close();
        return result;
}

std::vector<Card*> loadHand()
{
    std::string folderPath = "./card_jsons";

    std::vector<Card*> hand;
    for (const auto & entry : std::filesystem::directory_iterator(folderPath))
        {
            hand.push_back(loadCard(entry.path().string()));

        }
    return hand;
}
