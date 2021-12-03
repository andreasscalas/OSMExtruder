#include "osmrelation.h"

OSMRelation::OSMRelation()
{

}

std::string OSMRelation::getId() const
{
    return id;
}

void OSMRelation::setId(const std::string &value)
{
    id = value;
}

std::string OSMRelation::getUserName() const
{
    return user_name;
}

void OSMRelation::setUserName(const std::string &value)
{
    user_name = value;
}

std::string OSMRelation::getUserId() const
{
    return user_id;
}

void OSMRelation::setUserId(const std::string &value)
{
    user_id = value;
}

bool OSMRelation::getIsVisible() const
{
    return is_visible;
}

void OSMRelation::setIsVisible(bool value)
{
    is_visible = value;
}

std::string OSMRelation::getVersion() const
{
    return version;
}

void OSMRelation::setVersion(const std::string &value)
{
    version = value;
}

std::string OSMRelation::getChangeset() const
{
    return changeset;
}

void OSMRelation::setChangeset(const std::string &value)
{
    changeset = value;
}

std::vector<std::pair<std::string, std::string> > OSMRelation::getTags() const
{
    return tags;
}

void OSMRelation::setTags(const std::vector<std::pair<std::string, std::string> > &value)
{
    tags = value;
}

bool OSMRelation::checkTag(std::pair<std::string, std::string> tag)
{
    for(unsigned int i = 0; i < tags.size(); i++)
        if(tag.first == tags.at(i).first)
            return (tag.second.compare("") == 0 || tag.second.compare(tags.at(i).second) == 0);
    return false;
}

std::string OSMRelation::getTimestamp() const
{
    return timestamp;
}

void OSMRelation::setTimestamp(const std::string &value)
{
    timestamp = value;
}

std::string OSMRelation::getType() const
{
    return type;
}

void OSMRelation::setType(const std::string &value)
{
    type = value;
}

std::vector<std::pair<std::shared_ptr<OSMRelation>, std::string> > OSMRelation::getRelations() const
{
    return relations;
}

void OSMRelation::setRelations(const std::vector<std::pair<std::shared_ptr<OSMRelation>, std::string> > &value)
{
    relations = value;
}

bool OSMRelation::addRelation(const std::pair<std::shared_ptr<OSMRelation>, std::string> relation)
{
    relations.push_back(relation);
    return true;
}

std::vector<std::pair<std::shared_ptr<OSMNode>, std::string> > OSMRelation::getNodes() const
{
    return nodes;
}

void OSMRelation::setNodes(const std::vector<std::pair<std::shared_ptr<OSMNode>, std::string> > &value)
{
    nodes = value;
}

bool OSMRelation::addNode(const std::pair<std::shared_ptr<OSMNode>, std::string> node)
{
    nodes.push_back(node);
    return true;
}

std::vector<std::pair<std::shared_ptr<OSMWay>, std::string> > OSMRelation::getWays() const
{
    return ways;
}

void OSMRelation::setWays(const std::vector<std::pair<std::shared_ptr<OSMWay>, std::string> > &value)
{
    ways = value;
}

bool OSMRelation::addWay(const std::pair<std::shared_ptr<OSMWay>, std::string> way)
{
    ways.push_back(way);
    return true;
}
