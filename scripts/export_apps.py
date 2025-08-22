#!/usr/bin/env python3
import json
import csv
import os
from collections import OrderedDict
from typing import List, Dict, Any

CATALOG_JSON_PATH = "/workspace/catalog/apps_catalog.json"
CSV_OUTPUT_PATH = "/workspace/catalog/apps_catalog.csv"
MD_OUTPUT_PATH = "/workspace/docs/apps_index_by_category.md"


def load_catalog(json_path: str) -> List[Dict[str, Any]]:
    with open(json_path, "r", encoding="utf-8") as f:
        return json.load(f)


def group_apps_by_category(apps: List[Dict[str, Any]]) -> OrderedDict:
    category_to_apps: "OrderedDict[str, List[Dict[str, Any]]]" = OrderedDict()
    for app in apps:
        category = app.get("category", "未分组")
        if category not in category_to_apps:
            category_to_apps[category] = []
        category_to_apps[category].append(app)
    # Sort each category's apps by numeric id
    for category, items in category_to_apps.items():
        try:
            items.sort(key=lambda a: int(str(a.get("id", "app000"))[3:]))
        except Exception:
            items.sort(key=lambda a: str(a.get("id", "")))
    return category_to_apps


essential_csv_headers = [
    "id",
    "name",
    "category",
    "backend",
    "timeHours",
    "completeness",
    "mauEstimate",
    "apis",
    "coreFeatures",
]


def write_csv(apps: List[Dict[str, Any]], csv_path: str) -> None:
    os.makedirs(os.path.dirname(csv_path), exist_ok=True)
    with open(csv_path, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(essential_csv_headers)
        for app in apps:
            apis = ";".join(app.get("apis", []) or [])
            core_features = ";".join(app.get("coreFeatures", []) or [])
            row = [
                app.get("id", ""),
                app.get("name", ""),
                app.get("category", ""),
                app.get("backend", ""),
                app.get("timeHours", ""),
                app.get("completeness", ""),
                app.get("mauEstimate", ""),
                apis,
                core_features,
            ]
            writer.writerow(row)


def build_markdown_index(groups: OrderedDict) -> str:
    lines: List[str] = []
    lines.append("### 按类别分组的应用清单\n")
    for category, items in groups.items():
        lines.append(f"### {category}")
        for app in items:
            app_id = app.get("id", "")
            name = app.get("name", "")
            lines.append(f"- {app_id} {name}")
        lines.append("")
    return "\n".join(lines).rstrip() + "\n"


def write_markdown(md_content: str, md_path: str) -> None:
    os.makedirs(os.path.dirname(md_path), exist_ok=True)
    with open(md_path, "w", encoding="utf-8") as f:
        f.write(md_content)


def main() -> None:
    apps = load_catalog(CATALOG_JSON_PATH)
    groups = group_apps_by_category(apps)

    # Write CSV
    write_csv(apps, CSV_OUTPUT_PATH)

    # Build and write Markdown
    md_content = build_markdown_index(groups)
    write_markdown(md_content, MD_OUTPUT_PATH)

    # Print Markdown to stdout so the caller can display it
    print(md_content)


if __name__ == "__main__":
    main()