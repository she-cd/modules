# 项目文档结构

- `catalog/apps_catalog.json`: 200款应用的总目录（规格、估时、依赖、MAU预估）。
- `scripts/generate_app_docs.py`: 读取目录并批量生成每个应用的开发文档。
- `templates/app_spec_template.md.j2`: 单应用文档模板。
- `docs/apps/`: 自动生成的每个应用开发文档（Markdown）。
- `docs/global/development_plan.md`: 总体开发计划与流水线。
- `docs/global/resources.md`: 资源需求与外部服务配额。
- `docs/global/replacements.md`: 不合适应用的替换清单与理由。

## 生成单应用文档

```bash
python3 scripts/generate_app_docs.py --catalog catalog/apps_catalog.json --out docs/apps
```

可选参数：
- `--only <app_id>` 仅生成指定应用
- `--ids <id1,id2,...>` 批量指定

## 约定
- 时间估算为在已有通用模板和模块的净工时。
- `apis` 为 HarmonyOS 能力缩写；`backend` 为后端依赖级别（none/kv/cf/sb）。
- 生成的文档为 MVP 交付级，提交上架前请根据 UI 与法务要求补充截图与条款链接。