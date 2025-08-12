#!/usr/bin/env python3
import re
import json
import sys
from pathlib import Path

RE_LINE = re.compile(r"^\s*\d+\.\s*(?P<name>[^—]+?)\s*—\s*Core:\s*(?P<core>[^|]+)\|\s*APIs:\s*(?P<apis>[^|]+)\|\s*Backend:\s*(?P<backend>[^|]+)\|\s*Time:\s*(?P<time>[^|]+)\|\s*完整度:\s*(?P<completeness>[^|]+)\|\s*预估MAU:\s*(?P<mau>.+)$")

CATEGORY_RE = re.compile(r"^####\s*\d+\)\s*(?P<cat>.+)$")

REPLACEMENTS = {
    'HTTP 抓包（轻）': 'HTTP 请求调试台',
    '语音同传（简）': '短句语音翻译',
    '居家运动计划': '动作计时与训练计数',
    '酒店拍照量尺': '照片比例尺标注器',
    '公共交通等车计时': '通勤出发提醒',
    '红外遥控库（离线码）': '蓝牙遥控宏定义',
    '手持稳像（简版）': '视频画幅裁切助手',
}

SLUG_MAP = {
    'HTTP 请求调试台': 'http-request-console',
    '短句语音翻译': 'speech-snippet-translate',
    '动作计时与训练计数': 'workout-timer-counter',
    '照片比例尺标注器': 'photo-scale-annotator',
    '通勤出发提醒': 'commute-departure-reminder',
    '蓝牙遥控宏定义': 'bt-remote-macros',
    '视频画幅裁切助手': 'video-cropper',
}


def slugify(name: str) -> str:
    # simple slug, fallback to predefined map
    if name in SLUG_MAP:
        return SLUG_MAP[name]
    s = name.lower()
    s = re.sub(r"[\s/（）()]+", "-", s)
    s = re.sub(r"[^a-z0-9\-]+", "", s)
    s = re.sub(r"-+", "-", s).strip('-')
    if not s:
        s = 'app'
    return s


def parse():
    raw = Path('/workspace/catalog/raw_list.md').read_text(encoding='utf-8').splitlines()
    current_cat = ''
    catalog = []
    idx = 1
    for line in raw:
        mcat = CATEGORY_RE.match(line)
        if mcat:
            current_cat = mcat.group('cat').strip()
            continue
        m = RE_LINE.match(line)
        if not m:
            continue
        name = m.group('name').strip()
        if name in REPLACEMENTS:
            name = REPLACEMENTS[name]
        core = m.group('core').strip()
        apis = [a.strip() for a in m.group('apis').split(',')]
        backend = m.group('backend').strip()
        time = m.group('time').lower().replace('h', '').strip()
        completeness = m.group('completeness').replace('%', '').strip()
        mau = m.group('mau').strip()
        app_id = f"app{idx:03d}"
        slug = slugify(name)
        # defaults for doc generation
        app = {
            'id': app_id,
            'slug': slug,
            'name': name,
            'category': current_cat,
            'coreFeatures': [c.strip() for c in core.split('/')],
            'apis': apis if apis != ['None'] else [],
            'backend': {'无':'none','KV':'kv','CF':'cf','SB':'sb'}.get(backend, backend.lower()),
            'timeHours': int(float(time)) if time else 6,
            'completeness': int(float(completeness)) if completeness else 90,
            'mauEstimate': mau,
            'overview': f"围绕 {name} 的高频刚需，提供 {core} 的轻量解决方案，离线优先",
            'targetUsers': '普通用户/职场/学生（按需）',
            'scenarios': '日常高频场景，3秒完成一次核心操作',
            'competitors': '同类工具功能分散、广告多或重登陆',
            'features': [c.strip() for c in core.split('/')],
            'ux': {
                'pages': '主页/详情/设置（按需）',
                'states': '加载、空态、列表/编辑态',
                'cards': '原子化服务卡片（如适用）'
            },
            'dataModel': 'interface Item { id: string; title: string; createdAt: number; }',
            'permissions': [],
            'capabilityNotes': '按需申请并解释用途',
            'apisDesign': '轻服务或本地，无强后端',
            'cacheSync': 'Preferences 或 RDB 本地持久化',
            'quota': '本地优先；云接口日更缓存（如有）',
            'analytics': {
                'paths': ['首屏-核心操作-导出/分享'],
                'events': ['open','action_primary','share','retention_day7']
            },
            'backendNotes': 'none/kv/cf/sb 视功能需要',
            'risks': '权限/隐私/功耗',
            'mitigations': '最小权限、前后台策略、离线优先'
        }
        # permissions quick guess
        perm_map = {
            'Cam':'ohos.permission.CAMERA',
            'Files':'ohos.permission.READ_MEDIA',
            'Mic':'ohos.permission.MICROPHONE',
            'GPS':'ohos.permission.LOCATION',
            'Clipboard':'ohos.permission.PASTEBOARD',
            'Notifications':'ohos.permission.NOTIFICATION_CONTROLLER',
            'NFC':'ohos.permission.NFC_TAG',
            'BT':'ohos.permission.DISCOVER_BLUETOOTH',
        }
        perms = []
        for api in app['apis']:
            if api in perm_map:
                perms.append(perm_map[api])
        app['permissions'] = perms
        catalog.append(app)
        idx += 1
    return catalog


if __name__ == '__main__':
    catalog = parse()
    Path('/workspace/catalog/apps_catalog.json').write_text(json.dumps(catalog, ensure_ascii=False, indent=2), encoding='utf-8')
    print(f"Wrote {len(catalog)} items to /workspace/catalog/apps_catalog.json")