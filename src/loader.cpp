#include "../headers/loader.h"

#include "../libraries/json.hpp"
using json = nlohmann::json;

#include "../headers/card_text.h"
#include "../headers/enemyCards.h"
#include "../headers/gamestate.h"
#include "../headers/UI.h"

typedef std::unordered_map<std::string, json> JSONType; //type of a generic json

template<typename T>
T loadJsonField(const json& data, std::string key, T null) //attempts to find the data in "data" with key "key", returns "null" if its not found
{
    if (data.find(key) != data.end())
    {
        return data[key];
    }
    else
    {
        return null;
    }
}


ResourceStats loadStats(const json& data)
{
    ResourceStats stats;

    stats[Resource::coins.name].value = loadJsonField(data,"coins",0);
    stats[Resource::damage.name].value = loadJsonField(data,"damage",0);
    stats[Resource::food.name].value = loadJsonField(data,"food",0);

    return stats;
}

PlayerCard* loadPlayerCard(const json& data);

Choice* loadChoice(const json& data)
{
    ChoiceTypes type = Choice::stringToChoice[loadJsonField(data,"type",std::string(""))];

    switch (type)
    {
    case TRADE:
    case ATTACK:
        ResourceStats resources = loadStats(loadJsonField<json>(data,"stats",{}));
        CardRewards rewards;

        if (data.find("get") != data.end())
        {
            if (data["get"].type() == json::value_t::string) //only a single entry in rewards
            {
                rewards.push_back(std::shared_ptr<PlayerCard>(static_cast<PlayerCard*>(loadCard(loadJsonField(data,"get",std::string(""))))));
            }
            else if (data["get"].type() == json::value_t::array)
            {
                std::vector<std::string> unloaded = data["get"];
                for (auto card : unloaded) //load each reward
                {
                    rewards.push_back(std::shared_ptr<PlayerCard>(static_cast<PlayerCard*>(loadCard(card))));
                }
            }
        }
        Choice* ch =  (type == TRADE ? new Trade(resources,rewards) : new Attack(resources.getValue(Resource::damage.name),std::move(rewards),[](){
                                        MasterCardsUI::getUI()->addEnemyCardToDeck(new EnemyCard("Guard","sprites/cardfaces/guard.png",
                                                                                    {new Attack(10,{},{}),new HurtChoice(1)}
                                                                                                    ),true);
                                                    }));
        ch->setReshuffle(loadJsonField(data,"reshuffle",false));
        return ch;


    }
}

EnemyCard* loadEnemyCard(const json& data)
{
    std::string name = loadJsonField(data,"name",std::string(""));
    std::string spritePath = "./sprites/cardfaces/" +loadJsonField(data,"sprite",std::string("")) + ".png";
    std::vector<JSONType> choices = loadJsonField(data,"choices",std::vector<JSONType>{});

    std::vector<Choice*> loadedChoices;
    for (auto choice : choices)
    {
        loadedChoices.push_back(loadChoice(choice));
    }

    EnemyCard* enemy = new EnemyCard(name,spritePath,loadedChoices.begin(),loadedChoices.end());

    return enemy;
}

PlayerCard* loadPlayerCard(const json& data)
{
    PlayerCard* result = nullptr;
    if (data.find("stats") != data.end())
    {
        ResourceStats stats;
        stats.setValue(Resource::food.name,(data["stats"].find("food") != data["stats"].end()) ? (int)data["stats"]["food"] : 0);
        stats.setValue(Resource::coins.name,(data["stats"].find("coins") != data["stats"].end()) ? (int)data["stats"]["coins"] : 0);
        stats.setValue(Resource::damage.name,(data["stats"].find("damage") != data["stats"].end()) ? (int)data["stats"]["damage"] : 0);
        result = new PlayerCard(data["name"],"./sprites/cardfaces/" + std::string(data["sprite"]) + ".png",stats);

    }
    else
    {
        std::cerr << "loadPlayerCard: Somehow couldn't load data!\n";
    }
    return result;
}

BaseCard* loadCard(std::string jason)
{
        if (jason.size() <5 || jason.substr(jason.size() - 5, 5) != ".json") //check if ".json" is at the end
        {
            jason += ".json";
        }
        std::ifstream f(jason);
        if (!f.is_open())
        {
            f.open("card_jsons/cards/" + jason); //attempt to build path if the provided doesn't work
            if (!f.is_open())
            {
                f.open("card_jsons/enemies/" + jason);
                if (!f.is_open())
                {
                     std::cerr << "loadCard ERROR: failed to load " << jason << "\n";
                     exit(1);
                }
            }
        }
        json data = json::parse(f);
        BaseCard* result = nullptr;

        if (loadJsonField(data,"isEnemy",false)) //if is an enemy (the isEnemy field is provided AND is set to true)
        {
            result = loadEnemyCard(data);
        }
        else
        {
            result = loadPlayerCard(data);
        }

        f.close();
        return result;
}

BaseCard* loadCard(const CardInfo& info)
{
    return new PlayerCard(info.name,info.spritePath,info.stats);
}

std::vector<PlayerCard*> loadHand()
{
    std::string folderPath = "./card_jsons/cards/";

    std::vector<PlayerCard*> hand;
    for (const auto & entry : std::filesystem::directory_iterator(folderPath))
        {
            BaseCard* card = loadCard(entry.path().string());
            hand.push_back(static_cast<PlayerCard*>(card));

        }
    return hand;
}
