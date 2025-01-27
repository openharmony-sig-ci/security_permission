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

#include <thread>
#include <functional>
#include <iostream>

#include "gtest/gtest.h"

#include "distributed_permission_kit.h"
#include "distributed_permission_manager_client.h"
#include "distributed_permission_death_recipient.h"
#include "iservice_registry.h"
#include "ipc_skeleton.h"
#include "on_using_permission_reminder_stub.h"
#include "permission_log.h"
#include "system_ability_definition.h"

using namespace testing::ext;
using namespace OHOS::Security::Permission;
using namespace std;

namespace OHOS {
pid_t IPCSkeleton::pid_ = 1;
pid_t IPCSkeleton::uid_ = 1;
std::string IPCSkeleton::localDeviceId_ = "1004";
std::string IPCSkeleton::deviceId_ = "";
}  // namespace OHOS

class PermissionRemindInnerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        cout << "SetUpTestCase()" << endl;
    }

    static void TearDownTestCase()
    {
        cout << "TearDownTestCase()" << endl;
    }
    void SetUp()
    {
        cout << "SetUp() is running" << endl;
    }
    void TearDown()
    {
        cout << "TearDown()" << endl;
    }

    int CheckProxy()
    {
        OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
            OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            return -1;
        }

        OHOS::sptr<OHOS::IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(
            IDistributedPermission::SA_ID_DISTRIBUTED_PERMISSION_MANAGER_SERVICE);
        if (!remoteObject) {
            return -1;
        }

        OHOS::sptr<IDistributedPermission> distributedPermissionProxyTest;

        distributedPermissionProxyTest = OHOS::iface_cast<IDistributedPermission>(remoteObject);
        if ((!distributedPermissionProxyTest) || (!distributedPermissionProxyTest->AsObject())) {
            return -1;
        }

        OHOS::sptr<OHOS::IRemoteObject::DeathRecipient> recipientTest;
        recipientTest = new DistributedPermissionDeathRecipient();
        if (!recipientTest) {
            return -1;
        }
        return 0;
    }
};

class TestCallback : public OnUsingPermissionReminderStub {
public:
    TestCallback() = default;
    virtual ~TestCallback() = default;

    std::string permName = "ohos.permission.CAMERA";
    void StartUsingPermission(const PermissionReminderInfo &permReminderInfo)
    {
        EXPECT_EQ(permName, OHOS::Str16ToStr8(permReminderInfo.permName));
    }

    void StopUsingPermission(const PermissionReminderInfo &permReminderInfo)
    {
        EXPECT_EQ(permName, OHOS::Str16ToStr8(permReminderInfo.permName));
    }
};

HWTEST_F(PermissionRemindInnerTest, Test01, Function | MediumTest | Level1)
{
    OHOS::sptr<TestCallback> callback(new TestCallback());
    int ret = -1;
    int exRet = CheckProxy();

    string expect_j = "{\"deviceID\":\"1001\",\"pid\":1,\"uid\":2}";
    string permName = "ohos.permission.CAMERA";

    ret = DistributedPermissionKit::RegisterUsingPermissionReminder(callback);

    EXPECT_EQ(exRet, ret);

    ret = DistributedPermissionKit::CheckPermissionAndStartUsing(permName, expect_j);
    EXPECT_EQ(exRet, ret);

    ret = DistributedPermissionKit::CheckCallerPermissionAndStartUsing(permName);
    EXPECT_EQ(exRet, ret);

    DistributedPermissionKit::StartUsingPermission(permName, expect_j);

    DistributedPermissionKit::StopUsingPermission(permName, expect_j);

    ret = DistributedPermissionKit::UnregisterUsingPermissionReminder(callback);
    EXPECT_EQ(exRet, ret);
}

HWTEST_F(PermissionRemindInnerTest, Test02, Function | MediumTest | Level1)
{
    OHOS::sptr<TestCallback> callback;
    int ret = -1;
    int exRet = CheckProxy();
    string expect_j = "";
    string permName = "";

    ret = DistributedPermissionKit::RegisterUsingPermissionReminder(callback);
    EXPECT_EQ(-1, ret);

    ret = DistributedPermissionKit::CheckPermissionAndStartUsing(permName, expect_j);
    EXPECT_EQ(-1, ret);

    ret = DistributedPermissionKit::CheckCallerPermissionAndStartUsing(permName);
    EXPECT_EQ(exRet, ret);

    DistributedPermissionKit::StartUsingPermission(permName, expect_j);

    DistributedPermissionKit::StopUsingPermission(permName, expect_j);

    ret = DistributedPermissionKit::UnregisterUsingPermissionReminder(callback);
    EXPECT_EQ(-1, ret);
}
