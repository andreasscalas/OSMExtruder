#include "osmnode.h"

OSMNode::OSMNode()
{
    coordinates = std::make_shared<Point>();
}

std::vector<std::pair<std::string, std::string> > OSMNode::getTags() const
{
    return tags;
}

void OSMNode::setTags(const std::vector<std::pair<std::string, std::string> > &value)
{
    tags = value;
}

std::shared_ptr<Point> OSMNode::getCoordinates() const
{
    return coordinates;
}

void OSMNode::setCoordinates(const std::shared_ptr<Point> &value)
{
    coordinates->setInfo(value->getInfo());
    coordinates->setX(value->getX());
    coordinates->setY(value->getY());
    coordinates->setZ(value->getZ());
}

std::string OSMNode::getTimestamp() const
{
    return timestamp;
}

void OSMNode::setTimestamp(const std::string &value)
{
    timestamp = value;
}

std::string OSMNode::getId() const
{
    return id;
}

void OSMNode::setId(const std::string &value)
{
    id = value;
}

std::string OSMNode::getUserName() const
{
    return user_name;
}

void OSMNode::setUserName(const std::string &value)
{
    user_name = value;
}

std::string OSMNode::getUserId() const
{
    return user_id;
}

void OSMNode::setUserId(const std::string &value)
{
    user_id = value;
}

std::string OSMNode::getVersion() const
{
    return version;
}

void OSMNode::setVersion(const std::string &value)
{
    version = value;
}

std::string OSMNode::getChangeset() const
{
    return changeset;
}

void OSMNode::setChangeset(const std::string &value)
{
    changeset = value;
}

bool OSMNode::getIsVisible() const
{
    return is_visible;
}

void OSMNode::setIsVisible(bool value)
{
    is_visible = value;
}
