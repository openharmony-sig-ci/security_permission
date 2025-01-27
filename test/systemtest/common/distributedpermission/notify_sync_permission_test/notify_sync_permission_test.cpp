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
#include <cstdio>
#include <fstream>
#include <sstream>
#include <chrono>
#include <gtest/gtest.h>
#include <mutex>
#include "permission_log.h"
#include "system_test_distributed_permission_util.h"
#include "ipc_skeleton.h"
#include "time_util.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "ability_manager_interface.h"
#include "idistributed_permission.h"
#include "distributed_permission_manager_service.h"
#include "permission_kit.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::Permission;
using namespace OHOS::STPermissionUtil;
using namespace OHOS::AppExecFwk;
using std::string;

namespace {
static const std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
static const std::string permission_network = "ohos.permission.GET_NETWORK_INFO";
static const std::string permission_location = "ohos.permission.LOCATION_IN_BACKGROUND";
static const std::string permission_camera = "ohos.permission.CAMERA";
static const std::string permission_microphone = "ohos.permission.MICROPHONE";
static const std::string permission_myself1 = "ohos.permission.MYPERMISSION_1";
static const std::string permission_myself2 = "ohos.permission.MYPERMISSION_2";
static const std::string permission_buletooth = "ohos.permission.DISCOVER_BULETOOTH";
}  // namespace

class NotifySyncPermissionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NotifySyncPermissionTest::SetUpTestCase(void)
{}

void NotifySyncPermissionTest::TearDownTestCase(void)
{}

void NotifySyncPermissionTest::SetUp(void)
{}

void NotifySyncPermissionTest::TearDown(void)
{}

HWTEST_F(NotifySyncPermissionTest, DPMS_GetUidPermission_0100, TestSize.Level1)
{
    // DPMS_GetUidPermission_0100
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0100 start";
    int uid = STDistibutePermissionUtil::GetUidByBundleName(THIRD_BUNDLE_NAME);
    // 无效nodeId(nodeID为空)，主体主动同步权限信息时返回-3： 非法的设备标识符
    std::string nodeId = "";  // nodeId is null
    std::string packageName = "packageName";
    int result = DelayedSingleton<DistributedPermissionManagerService>::GetInstance()->NotifySyncPermission(
        nodeId, uid, packageName);
    EXPECT_EQ(result, -3);
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0100 end";
}

HWTEST_F(NotifySyncPermissionTest, DPMS_GetUidPermission_0200, TestSize.Level1)
{
    // DPMS_GetUidPermission_0200
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0200 start";
    int uid = STDistibutePermissionUtil::GetUidByBundleName(THIRD_BUNDLE_NAME);
    // 无效nodeId(nodeId长度>64)，主体主动同步权限信息时返回-3： 非法的设备标识符
    std::string nodeId = "12345678901234567890123456789012345678901234567890123456789012345";
    std::string packageName = "packageName";
    int result = DelayedSingleton<DistributedPermissionManagerService>::GetInstance()->NotifySyncPermission(
        nodeId, uid, packageName);
    EXPECT_EQ(result, -3);
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0200 end";
}

HWTEST_F(NotifySyncPermissionTest, DPMS_GetUidPermission_0300, TestSize.Level1)
{
    // DPMS_GetUidPermission_0300
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0300 start";
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        "networkId", "universallyUniqueId", "uniqueDisabilityIdSuccess", "deviceName", "deviceType");
    int uid = STDistibutePermissionUtil::GetUidByBundleName(THIRD_BUNDLE_NAME);
    std::string nodeId = "networkId";
    std::string packageName = "packageName";
    // 无效UID(UID<0)，主体主动同步权限信息时返回-1: 其他错误
    uid = -100;
    int result = DelayedSingleton<DistributedPermissionManagerService>::GetInstance()->NotifySyncPermission(
        nodeId, uid, packageName);
    EXPECT_EQ(result, -1);
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0300 end";
}

HWTEST_F(NotifySyncPermissionTest, DPMS_GetUidPermission_0400, TestSize.Level1)
{
    // DPMS_GetUidPermission_0400
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0400 start";
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        "networkId", "universallyUniqueId", "uniqueDisabilityIdSuccess", "deviceName", "deviceType");

    int uid = STDistibutePermissionUtil::GetUidByBundleName(THIRD_BUNDLE_NAME);
    std::string nodeId = "networkId";
    // 空的packageName，主体主动同步权限信息时返回-1: 其他错误
    std::string packageName = "";

    int result = DelayedSingleton<DistributedPermissionManagerService>::GetInstance()->NotifySyncPermission(
        nodeId, uid, packageName);

    EXPECT_EQ(result, -1);

    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0400 end";
}

HWTEST_F(NotifySyncPermissionTest, DPMS_GetUidPermission_0500, TestSize.Level1)
{
    // DPMS_GetUidPermission_0500
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0500 start";

    // 安装应用包的升级包。
    // 应用包的权限数量超过1024，主体主动同步权限信息时同步失败
    // permission GrantMode::SYSTEM_GRANT == permission.grantMode 1025
    GTEST_LOG_(INFO) << "ReInstall THIRD_EQ_MAXPERMISSION_HAP_NAME start.";
    STDistibutePermissionUtil::ReInstall(THIRD_EQ_MAXPERMISSION_HAP_NAME);
    GTEST_LOG_(INFO) << "ReInstall THIRD_EQ_MAXPERMISSION_HAP_NAME end.";
    std::string bundleName = THIRD_EQ_MAXPERMISSION_BUNDLE_NAME;
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    std::string permission_name = "com.myability.permission.MYPERMISSION_";

    for (int32_t i = 0; i <= 1024; i++) {
        std::string permission_temp = permission_name + std::to_string(i);
        OHOS::Security::Permission::PermissionDef permissionDef_temp = {.permissionName = permission_temp,
            .bundleName = bundleName,
            .grantMode = 1,
            .availableScope = 1 << 0,
            .label = "test label",
            .labelId = 9527,
            .description = "test description",
            .descriptionId = 9528};
        permDefList.emplace_back(permissionDef_temp);
    }

    PermissionKit::AddDefPermissions(permDefList);
    std::vector<std::string> permList_system;
    for (int32_t i = 0; i <= 1024; i++) {
        std::string permission_temp = permission_name + std::to_string(i);
        permList_system.push_back(permission_temp);
    }
    PermissionKit::AddSystemGrantedReqPermissions(bundleName, permList_system);

    for (int32_t i = 0; i <= 1024; i++) {
        std::string permission_temp = permission_name + std::to_string(i);
        PermissionKit::GrantSystemGrantedPermission(bundleName, permission_temp);
    }
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0500 end";
}

HWTEST_F(NotifySyncPermissionTest, DPMS_GetUidPermission_0600, TestSize.Level1)
{
    // DPMS_GetUidPermission_0600 应用包的权限数量超过1024，主体主动同步权限信息时同步失败
    // DPMS_GetUidPermission_2100 接口性能
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0600 start";

    GTEST_LOG_(INFO) << "Install THIRD_EQ_MAXPERMISSION_HAP_NAME start.";
    STDistibutePermissionUtil::Install(THIRD_MORETHAN_MAXPERMISSION_HAP_NAME);
    GTEST_LOG_(INFO) << "Install THIRD_EQ_MAXPERMISSION_HAP_NAME end.";

    auto timeFast = std::chrono::milliseconds(100000);
    std::this_thread::sleep_for(timeFast);

    GTEST_LOG_(INFO) << "Uninstall THIRD_EQ_MAXPERMISSION_HAP_NAME start.";
    STDistibutePermissionUtil::Uninstall("com.third.hiworld.dpmsThirddMaxPermissionExceeded");
    GTEST_LOG_(INFO) << "Uninstall THIRD_EQ_MAXPERMISSION_HAP_NAME end.";

    GTEST_LOG_(INFO) << "ReInstall THIRD_EQ_MAXPERMISSION_HAP_NAME start.";
    STDistibutePermissionUtil::ReInstall(THIRD_MORETHAN_MAXPERMISSION_HAP_NAME);
    GTEST_LOG_(INFO) << "ReInstall THIRD_EQ_MAXPERMISSION_HAP_NAME end.";

    std::string bundleName = THIRD_EQ_MAXPERMISSION_BUNDLE_NAME;
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    std::string permission_name = "com.myability.permission.MYPERMISSION_";

    for (int32_t i = 0; i < 1024; i++) {
        std::string permission_temp = permission_name + std::to_string(i);
        OHOS::Security::Permission::PermissionDef permissionDef_temp = {.permissionName = permission_temp,
            .bundleName = bundleName,
            .grantMode = 1,
            .availableScope = 1 << 0,
            .label = "test label",
            .labelId = 9527,
            .description = "test description",
            .descriptionId = 9528};
        permDefList.emplace_back(permissionDef_temp);
    }

    PermissionKit::AddDefPermissions(permDefList);
    std::vector<std::string> permList_system;
    for (int32_t i = 0; i <= 1024; i++) {
        std::string permission_temp = permission_name + std::to_string(i);
        permList_system.push_back(permission_temp);
    }
    PermissionKit::AddSystemGrantedReqPermissions(bundleName, permList_system);

    for (int32_t i = 0; i <= 1024; i++) {
        std::string permission_temp = permission_name + std::to_string(i);
        PermissionKit::GrantSystemGrantedPermission(bundleName, permission_temp);
    }

    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0600 end";
}

/**
 * 三方应用
 */
HWTEST_F(NotifySyncPermissionTest, DPMS_GetUidPermission_0700, TestSize.Level1)
{
    // DPMS_GetUidPermission_0700 主体主动同步权限信息时，“只允许本地应用使用”的权限不同步
    // DPMS_GetUidPermission_0800 主体主动同步权限信息时，三方应用，user_grant权限不同步
    // DPMS_GetUidPermission_1000 主体主动同步权限信息时，三方应用，system_grant权限同步
    // DPMS_GetUidPermission_1200 主体主动同步权限信息时，三方自定义的权限，不同步
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0700 start";

    GTEST_LOG_(INFO) << "Install dpmsThirdIncludeUseByLocalAppAddV1 start.";
    STDistibutePermissionUtil::Uninstall("com.third.hiworld.include_use_by_local_app");
    GTEST_LOG_(INFO) << "Install dpmsThirdIncludeUseByLocalAppAddV1 end.";

    GTEST_LOG_(INFO) << "Install dpmsThirdIncludeUseByLocalAppAddV1 start.";
    STDistibutePermissionUtil::ReInstall("dpmsThirdIncludeUseByLocalAppAddV1");
    GTEST_LOG_(INFO) << "Install dpmsThirdIncludeUseByLocalAppAddV1 end.";

    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0700 end";
}

HWTEST_F(NotifySyncPermissionTest, DPMS_GetUidPermission_0800, TestSize.Level1)
{
    // DPMS_GetUidPermission_1600 在主体三方应用的包权限减少时，调用NotifySyncPermission同步权限给客体
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0800 start";

    GTEST_LOG_(INFO) << "Uninstall dpmsThirdIncludeUseByLocalAppAddV1 start.";
    STDistibutePermissionUtil::Uninstall("com.third.hiworld.include_use_by_local_app");
    GTEST_LOG_(INFO) << "Uninstall dpmsThirdIncludeUseByLocalAppAddV1 end.";

    // 安装应用包的升级包。
    // 主体主动同步权限信息时，“只允许本地应用使用”的权限不同步
    GTEST_LOG_(INFO) << "ReInstall dpmsThirdIncludeUseByLocalAppAddV1 start.";
    STDistibutePermissionUtil::ReInstall("dpmsThirdIncludeUseByLocalAppAddV1");
    GTEST_LOG_(INFO) << "ReInstall dpmsThirdIncludeUseByLocalAppAddV1 end.";

    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0800 end";
}

HWTEST_F(NotifySyncPermissionTest, DPMS_GetUidPermission_0900, TestSize.Level1)
{
    // DPMS_GetUidPermission_1400 在主体三方应用的包权限增加时，调用NotifySyncPermission同步权限给客体

    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0800 start";

    GTEST_LOG_(INFO) << "Uninstall dpmsThirdIncludeUseByLocalAppAddV1 start.";
    STDistibutePermissionUtil::Uninstall("com.third.hiworld.include_use_by_local_app");
    GTEST_LOG_(INFO) << "Uninstall dpmsThirdIncludeUseByLocalAppAddV1 end.";

    GTEST_LOG_(INFO) << "ReInstall dpmsThirdIncludeUseByLocalAppAddV1 start.";
    STDistibutePermissionUtil::ReInstall("dpmsThirdIncludeUseByLocalAppAddV1");
    GTEST_LOG_(INFO) << "ReInstall dpmsThirdIncludeUseByLocalAppAddV1 end.";

    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_0900 end";
}

/**
 * 系统应用
 */
HWTEST_F(NotifySyncPermissionTest, DPMS_GetUidPermission_1000, TestSize.Level1)
{
    // DPMS_GetUidPermission_0700 主体主动同步权限信息时，“只允许本地应用使用”的权限不同步
    // DPMS_GetUidPermission_0900 主体主动同步权限信息时，系统应用，system_grant权限同步
    // DPMS_GetUidPermission_1100 主体主动同步权限信息时，系统应用，授权的user_grant权限同步

    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_1000 start";
    std::string bundleName = "com.system.hiworld.include_use_by_local_app";
    GTEST_LOG_(INFO) << "Install dpmsSystemIncludeUseByLocalApp start.";
    STDistibutePermissionUtil::ReInstall("dpmsSystemIncludeUseByLocalAppAddV1");
    GTEST_LOG_(INFO) << "Install dpmsSystemIncludeUseByLocalApp end.";

    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_1000 end";
}

HWTEST_F(NotifySyncPermissionTest, DPMS_GetUidPermission_1100, TestSize.Level1)
{
    // DPMS_GetUidPermission_1500 在主体系统应用包权限减少时，调用NotifySyncPermission同步权限给客体
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_1100 start";

    std::string bundleName = "com.system.hiworld.include_use_by_local_app";

    GTEST_LOG_(INFO) << "Install dpmsSystemIncludeUseByLocalApp start.";
    STDistibutePermissionUtil::ReInstall("dpmsSystemIncludeUseByLocalAppAddV1");
    GTEST_LOG_(INFO) << "Install dpmsSystemIncludeUseByLocalApp end.";

    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_1100 end";
}

HWTEST_F(NotifySyncPermissionTest, DPMS_GetUidPermission_1200, TestSize.Level1)
{
    // 在主体系统应用包权限增加时，调用NotifySyncPermission同步权限给客体
    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_1200 start";

    std::string bundleName = "com.system.hiworld.include_use_by_local_app";
    GTEST_LOG_(INFO) << "Install dpmsSystemIncludeUseByLocalApp start.";
    STDistibutePermissionUtil::ReInstall("dpmsSystemIncludeUseByLocalAppAddV1");
    GTEST_LOG_(INFO) << "Install dpmsSystemIncludeUseByLocalApp end.";

    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    // system grant
    OHOS::Security::Permission::PermissionDef permissionDef_network = {.permissionName = permission_network,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_wifi = {.permissionName = permission_wifi,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_location = {.permissionName = permission_location,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    // user grant
    OHOS::Security::Permission::PermissionDef permissionDef_camera = {.permissionName = permission_camera,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_microphone = {.permissionName = permission_microphone,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    permDefList.emplace_back(permissionDef_network);
    permDefList.emplace_back(permissionDef_wifi);
    permDefList.emplace_back(permissionDef_location);
    permDefList.emplace_back(permissionDef_camera);
    permDefList.emplace_back(permissionDef_microphone);
    PermissionKit::AddDefPermissions(permDefList);
    // system grant
    std::vector<std::string> permList_system;
    permList_system.push_back(permission_network);
    permList_system.push_back(permission_wifi);
    permList_system.push_back(permission_location);
    PermissionKit::AddSystemGrantedReqPermissions(bundleName, permList_system);
    PermissionKit::GrantSystemGrantedPermission(bundleName, permission_network);
    PermissionKit::GrantSystemGrantedPermission(bundleName, permission_wifi);
    PermissionKit::GrantSystemGrantedPermission(bundleName, permission_location);
    // user grant
    std::vector<std::string> permList_user;
    permList_user.push_back(permission_camera);
    permList_user.push_back(permission_microphone);
    PermissionKit::AddUserGrantedReqPermissions(bundleName, permList_user, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_camera, 0);

    GTEST_LOG_(INFO) << "NotifySyncPermissionTest DPMS_GetUidPermission_1200 end";
}
// NotifySyncPermission稳定性
// NotifySyncPermission接口Fuzz