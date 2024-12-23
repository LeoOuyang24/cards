#ifndef DECK_H_INCLUDED
#define DECK_H_INCLUDED

#include <list>

#include "render.h"

typedef std::string ResourceID; //identifier for a resource

struct Resource //a single resource
{
    int value = 0;
    ResourceID name = "";
    char icon = '\0';

    //predefined resources, copy them
    const static Resource coins;
    const static Resource damage;
    const static Resource food;
};




//represents how much resource each card provides
//may also represent how much resource is needed for a trade
struct ResourceStats
{
    //in the future, maybe the key should be an enum
    //for now I think it's easier for it to be a string
    std::unordered_map<ResourceID,Resource> resources;

    int getValue(ResourceID name) const//get value of a resource
    {
        return resources.find(name) != resources.end() ? resources.at(name).value : 0;
    }

    void setValue(ResourceID name, int val)
    {
        resources[name].value = val;
    }

    ResourceStats()
    {
        resources[Resource::coins.name] = Resource::coins;
        resources[Resource::damage.name] = Resource::damage;
        resources[Resource::food.name] = Resource::food;
    }
    ResourceStats(std::initializer_list<int> nums) : ResourceStats()
    {
        int i = 0;
        for (auto it : nums)
        {
            resources[i == 0 ? Resource::coins.name : (i == 1 ? Resource::damage.name : Resource::food.name)].value = it;
            i++;
            if (i >= 3)
            {
                break;
            }
        }
    }

    Resource& operator[](ResourceID str)
    {
        return resources[str];
    }

    //run a lambda on every pair in the resources. ( Resource&) -> void, (Resource&) -> bool
    //if the lambda returns a false, it ends early
    //isConst = true if the function doesn't modify the original object
    template<typename T>
    void forEach(T func)
    {
        for (auto& [key, value] : resources)
        {
            if constexpr(std::is_same<decltype(func),bool>::value)
            {
                if (!func(value))
                {
                    break;
                }
            }
            else
            {
                func(value);
            }
        }
    }
    //same as above but const
    template<typename T>
    void forEach(T func) const
    {
        for ( const auto& [key, value] : resources)
        {
            if constexpr(std::is_same<decltype(func),bool>::value)
            {
                if (!func(value))
                {
                    break;
                }
            }
            else
            {
                func(value);
            }
        }
    }
    std::string toString(bool oneline = true)
    {
        std::string message = "[";
        forEach([&message,oneline](Resource& resource){
                message += resource.name + ": " + std::to_string(resource.value) + (oneline ? ", " : "\n");
                });
        message += "]";

        return message;
    }
    ResourceStats operator+(const ResourceStats& b) const
    {
        ResourceStats n = b;
        n.resources.insert(resources.begin(),resources.end());
        n.forEach([this,&b,&n](Resource& resource){
                n.resources[resource.name].value = b.getValue(resource.name) + getValue(resource.name);
                });
        return n;
    }
    ResourceStats operator*(int num) const    {
        ResourceStats n;
       n. forEach([this,num,&n](Resource& resource){
                n.resources[resource.name].value = resource.value*num;
                });
        return n;
    }
    ResourceStats operator-(const ResourceStats& b) const
    {
        return (*this ) + b*-1;
    }
    void operator += (const ResourceStats& a)
    {
        (*this) = (*this) + a;
    }
    void operator -= (const ResourceStats& a)
    {
        (*this) = (*this) - a;
    }
    bool operator == (const ResourceStats& a)
    {
        bool t = true;
        forEach([this,&a,&t](Resource& resource){
                t = (resource.value == a.getValue(resource.name));
                return t;
                });
    }
    bool operator >= (const ResourceStats& a)
    {
        bool t = true;
        forEach([this,&a,&t](Resource& resource){
                t = t && (resource.value >= a.getValue(resource.name));
                return t;
                });
        return t;
    }
    bool operator <= (const ResourceStats& a)
    {
        bool t = true;
        forEach([this,&a,&t](Resource& resource){
                t = t && (resource.value <= a.getValue(resource.name));
                return t;
                });
        return t;
    }

};

class Card
{
    std::shared_ptr<Sprite> sprite;
    std::string text = "";
    std::string name = "";

    ResourceStats stats;
public:
    //create the card
    //if no sprite is given, get a random one
    Card(std::string name, std::string spritePath = "", std::string text_ = "Deal \x80 Damage");
    Card(std::string name, std::string spritePath, const ResourceStats& stats);
    Sprite* getSprite() const;
    std::string getText() const;
    std::string getName() const;
    ResourceStats getStats() const;
};

//data structure that represents a deck
//top-most card is at the end of the vector
//this makes popping it a lot more efficient
typedef std::shared_ptr<Card> CardPtr;
typedef std::weak_ptr<Card> CardWeakPtr;


typedef std::list<CardWeakPtr> HandType;

/*class Hand
{
    HandType hand;
public:
    //return the card(s) that we drew
    Hand()
    {

    }

    //provide a starting hand. Make sure the starting hand container is very shortlived because we will be std::moving it
    Hand(const HandType& starting);
    std::vector<CardPtr> draw(Deck& deck, int amount = 1);
    HandType& getHand();
};*/

#endif // DECK_H_INCLUDED
