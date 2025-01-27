/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <sys/types.h>
#include <unistd.h>
#include "distributed_permission_kit.h"

namespace OHOS {
namespace Security {
namespace Permission {
int32_t DistributedPermissionKit::AllocateDuid(const std::string &nodeId, const int32_t rUid)
{
    return DistributedPermissionManagerClient::GetInstance().AllocateDuid(nodeId, rUid);
}

int32_t DistributedPermissionKit::QueryDuid(const std::string &deviceId, int32_t rUid)
{
    return DistributedPermissionManagerClient::GetInstance().QueryDuid(deviceId, rUid);
}

bool DistributedPermissionKit::IsDuid(int32_t uid)
{
    int32_t userId = uid / Constant::PER_USER_RANGE;
    return userId == Constant::DISTRIBUTED_USER_ID;
}

std::string DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(pid_t pid, uid_t uid)
{
    nlohmann::json appIdInfo;
    appIdInfo["pid"] = pid;
    appIdInfo["uid"] = uid;
    return appIdInfo.dump();
}

std::string DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(
    pid_t pid, uid_t uid, const std::string &deviceID)
{
    nlohmann::json appIdInfo;
    appIdInfo["pid"] = pid;
    appIdInfo["uid"] = uid;
    if (!deviceID.empty()) {
        appIdInfo["deviceID"] = deviceID;
    }
    return appIdInfo.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);
}

std::string DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(
    pid_t pid, uid_t uid, const std::string &deviceID, const std::string &bundleName)
{
    nlohmann::json appIdInfo;
    appIdInfo["pid"] = pid;
    appIdInfo["uid"] = uid;
    if (!deviceID.empty()) {
        appIdInfo["deviceID"] = deviceID;
    }
    if (!bundleName.empty()) {
        appIdInfo["bundleName"] = bundleName;
    }
    return appIdInfo.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);
}

static void ParsePid(const nlohmann::json &jsonObject, int32_t &pid)
{
    if (jsonObject.find("pid") != jsonObject.end() && jsonObject.at("pid").is_number()) {
        pid = jsonObject.at("pid").get<int32_t>();
    }
}

static void ParseUid(const nlohmann::json &jsonObject, int32_t &uid)
{
    if (jsonObject.find("uid") != jsonObject.end() && jsonObject.at("uid").is_number()) {
        uid = jsonObject.at("uid").get<int32_t>();
    }
}

static void ParseDeviceID(const nlohmann::json &jsonObject, std::string &deviceID)
{
    if (jsonObject.find("deviceID") != jsonObject.end() && jsonObject.at("deviceID").is_string()) {
        deviceID = jsonObject.at("deviceID").get<std::string>();
    }
}

bool DistributedPermissionKit::AppIdInfoHelper::ParseAppIdInfo(
    const std::string &appIdInfo, Permission::AppIdInfo &appIdInfoObj)
{
    nlohmann::json jsonObj = nlohmann::json::parse(appIdInfo, nullptr, false);
    if (jsonObj.is_discarded()) {
        return false;
    }
    int32_t pid = Constant::INVALID_ID;
    int32_t uid = Constant::INVALID_ID;
    std::string deviceID;
    ParsePid(jsonObj, pid);
    ParseUid(jsonObj, uid);
    ParseDeviceID(jsonObj, deviceID);
    appIdInfoObj.pid = pid;
    appIdInfoObj.uid = uid;
    appIdInfoObj.deviceId = deviceID;
    return appIdInfoObj.Valid();
}

int32_t DistributedPermissionKit::CheckDPermission(int32_t dUid, const std::string &permissionName)
{
    return DistributedPermissionManagerClient::GetInstance().CheckDPermission(dUid, permissionName);
}

int32_t DistributedPermissionKit::CheckPermission(const std::string &permissionName, const std::string &appIdInfo)
{
    return DistributedPermissionManagerClient::GetInstance().CheckPermission(permissionName, appIdInfo);
}

int32_t DistributedPermissionKit::CheckSelfPermission(const std::string &permissionName)
{
    return DistributedPermissionManagerClient::GetInstance().CheckSelfPermission(permissionName);
}

int32_t DistributedPermissionKit::CheckCallingPermission(const std::string &permissionName)
{
    return DistributedPermissionManagerClient::GetInstance().CheckCallingPermission(permissionName);
}

int32_t DistributedPermissionKit::CheckCallingOrSelfPermission(const std::string &permissionName)
{
    return DistributedPermissionManagerClient::GetInstance().CheckCallingOrSelfPermission(permissionName);
}

int32_t DistributedPermissionKit::CheckCallerPermission(const std::string &permissionName)
{
    return DistributedPermissionManagerClient::GetInstance().CheckCallerPermission(permissionName);
}

bool DistributedPermissionKit::IsRestrictedPermission(const std::string &permissionName)
{
    return DistributedPermissionManagerClient::GetInstance().IsRestrictedPermission(permissionName);
}
int32_t DistributedPermissionKit::VerifyPermissionFromRemote(
    const std::string &permission, const std::string &nodeId, const std::string &appIdInfo)
{
    return DistributedPermissionManagerClient::GetInstance().VerifyPermissionFromRemote(permission, nodeId, appIdInfo);
}
int32_t DistributedPermissionKit::VerifySelfPermissionFromRemote(
    const std::string &permissionName, const std::string &nodeId)
{
    return DistributedPermissionManagerClient::GetInstance().VerifySelfPermissionFromRemote(permissionName, nodeId);
}

bool DistributedPermissionKit::CanRequestPermissionFromRemote(
    const std::string &permissionName, const std::string &nodeId)
{
    return DistributedPermissionManagerClient::GetInstance().CanRequestPermissionFromRemote(permissionName, nodeId);
}
void DistributedPermissionKit::RequestPermissionsFromRemote(const std::vector<std::string> permissions,
    const sptr<OnRequestPermissionsResult> &callback, const std::string &nodeId, const std::string &bundleName,
    int32_t reasonResId)
{
    return DistributedPermissionManagerClient::GetInstance().RequestPermissionsFromRemote(
        permissions, callback, nodeId, bundleName, reasonResId);
}

void DistributedPermissionKit::GrantSensitivePermissionToRemoteApp(
    const std::string &permissionName, const std::string &nodeId, int32_t ruid)
{
    return DistributedPermissionManagerClient::GetInstance().GrantSensitivePermissionToRemoteApp(
        permissionName, nodeId, ruid);
}

int32_t DistributedPermissionKit::RegisterUsingPermissionReminder(const sptr<OnUsingPermissionReminder> &callback)
{
    return DistributedPermissionManagerClient::GetInstance().RegisterUsingPermissionReminder(callback);
}

int32_t DistributedPermissionKit::UnregisterUsingPermissionReminder(const sptr<OnUsingPermissionReminder> &callback)
{
    return DistributedPermissionManagerClient::GetInstance().UnregisterUsingPermissionReminder(callback);
}

int32_t DistributedPermissionKit::CheckPermissionAndStartUsing(
    const std::string &permissionName, const std::string &appIdInfo)
{
    return DistributedPermissionManagerClient::GetInstance().CheckPermissionAndStartUsing(permissionName, appIdInfo);
}

int32_t DistributedPermissionKit::CheckCallerPermissionAndStartUsing(const std::string &permissionName)
{
    return DistributedPermissionManagerClient::GetInstance().CheckCallerPermissionAndStartUsing(permissionName);
}

void DistributedPermissionKit::StartUsingPermission(const std::string &permName, const std::string &appIdInfo)
{
    return DistributedPermissionManagerClient::GetInstance().StartUsingPermission(permName, appIdInfo);
}

void DistributedPermissionKit::StopUsingPermission(const std::string &permName, const std::string &appIdInfo)
{
    return DistributedPermissionManagerClient::GetInstance().StopUsingPermission(permName, appIdInfo);
}

void DistributedPermissionKit::AddPermissionUsedRecord(
    const std::string &permissionName, const std::string &appIdInfo, const int32_t sucCount, const int32_t failCount)
{
    return DistributedPermissionManagerClient::GetInstance().AddPermissionUsedRecord(
        permissionName, appIdInfo, sucCount, failCount);
}

int32_t DistributedPermissionKit::GetPermissionUsedRecords(
    const QueryPermissionUsedRequest &request, QueryPermissionUsedResult &result)
{
    return DistributedPermissionManagerClient::GetInstance().GetPermissionUsedRecords(request, result);
}

int32_t DistributedPermissionKit::GetPermissionUsedRecords(
    const QueryPermissionUsedRequest &request, const sptr<OnPermissionUsedRecord> &callback)
{
    return DistributedPermissionManagerClient::GetInstance().GetPermissionUsedRecords(request, callback);
}

}  // namespace Permission
}  // namespace Security
}  // namespace OHOS