#include "ScoreStorageProxy.h"

#include "base/Log.h"
#include "base/ObjectSerializer.h"

ScoreStorageProxy::ScoreStorageProxy() {
    _tableName = "Score";

    _columnsNameAndType["points"] = "int";
    _columnsNameAndType["level"] = "int";
    _columnsNameAndType["time"] = "float";
    _columnsNameAndType["name"] = "string";
    _columnsNameAndType["mode"] = "int";
    _columnsNameAndType["difficulty"] = "int";
}

std::string ScoreStorageProxy::getValue(const std::string& columnName) {
    if (columnName == "points") {
        return ObjectSerializer<int>::object2string(_queue.back().points);
    } else if (columnName == "level") {
        return ObjectSerializer<int>::object2string(_queue.back().level);
    } else if (columnName == "time") {
        return ObjectSerializer<float>::object2string(_queue.back().time);
    } else if (columnName == "name") {
        return _queue.back().name;
    } else if (columnName == "mode") {
        return ObjectSerializer<int>::object2string(_queue.back().mode);
    } else if (columnName == "difficulty") {
        return ObjectSerializer<int>::object2string(_queue.back().difficulty);
    } else {
        LOGW("No such column name: " << columnName);
    }
    return "";
}

void ScoreStorageProxy::setValue(const std::string& columnName, const std::string& value) {
    if (columnName == "points") {
        _queue.back().points =  ObjectSerializer<int>::string2object(value);
    } else if (columnName == "level") {
        _queue.back().level = ObjectSerializer<int>::string2object(value);
    } else if (columnName == "time") {
        _queue.back().time = ObjectSerializer<float>::string2object(value);
    } else if (columnName == "name") {
        _queue.back().name = value;
    } else if (columnName == "mode") {
        _queue.back().mode = (GameMode)ObjectSerializer<int>::string2object(value);
    } else if (columnName == "difficulty") {
        _queue.back().difficulty = (Difficulty)ObjectSerializer<int>::string2object(value);
    } else {
        LOGW("No such column name: " << columnName);
    }
}

