/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed as defined on the LICENSE file found in the
 *  root directory of this source tree.
 */

#include <osquery/core/windows/wmi.h>
#include <osquery/logger.h>
#include <osquery/sql.h>
#include <osquery/system.h>
#include <osquery/tables.h>

namespace osquery {
namespace tables {

QueryData genHVCIStatus(QueryContext& context) {
  QueryData results;

  std::vector<std::string> vbs_methods = {"VBS_NOT_ENABLED",
                                          "VBS_ENABLED_AND_NOT_RUNNING",
                                          "VBS_ENABLED_AND_RUNNING"};

  std::vector<std::string> enforcement_methods = {
      "OFF", "AUDIT_MODE", "ENFORCED_MODE"};

  const WmiRequest wmiSystemReq("SELECT * FROM Win32_DeviceGuard",
                                (BSTR)L"ROOT\\MICROSOFT\\WINDOWS\\DEVICEGUARD");
  const std::vector<WmiResultItem>& wmiResults = wmiSystemReq.results();
  if (wmiResults.empty()) {
    LOG(ERROR) << "Error retreiving information from WMI.";
    return results;
  }
  for (const auto& data : wmiResults) {
    Row r;
    data.GetString("Version", r["version"]);
    data.GetString("InstanceIdentifier", r["instance_identifier"]);

    long vbs_status;
    data.GetLong("VirtualizationBasedSecurityStatus", vbs_status);
    r["vbs_status"] =
        vbs_methods.size() < vbs_status ? vbs_methods[vbs_status] : "UNKNOWN";

    long code_policy_status;
    data.GetLong("CodeIntegrityPolicyEnforcementStatus", code_policy_status);
    r["code_integrity_policy_enforcement_status"] =
        enforcement_methods.size() < code_policy_status
            ? enforcement_methods[vbs_status]
            : "UNKNOWN";

    long umci_status;
    data.GetLong("UsermodeCodeIntegrityPolicyEnforcementStatus", umci_status);
    r["umci_policy_status"] = enforcement_methods.size() < umci_status
                                  ? enforcement_methods[umci_status]
                                  : "UNKNOWN";

    results.push_back(r);
  }
  return results;
}
} // namespace tables
} // namespace osquery
