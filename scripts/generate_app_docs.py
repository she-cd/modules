#!/usr/bin/env python3
import argparse
import json
import os
from datetime import datetime

TEMPLATE_PATH = os.path.join(os.path.dirname(__file__), '..', 'templates', 'app_spec_template.md.j2')


def render(template: str, ctx: dict) -> str:
    # Minimal templating for {{var}} and simple loops for features
    from string import Template
    # Convert simple placeholders using ${}
    # We'll pre-expand the features list and a few nested fields manually
    text = template
    # features loop
    if '{{features' in text:
        loop_block_start = text.find('{% for f in features %}')
        loop_block_end = text.find('{% endfor %}', loop_block_start)
        loop_block = text[loop_block_start:loop_block_end+len('{% endfor %}')]
        item_tpl = loop_block.split('%}')[1].split('{%')[0]
        rendered_items = ''.join(item_tpl.replace('{{f}}', f) for f in ctx.get('features', []))
        text = text.replace(loop_block, rendered_items)
    # simple replace for a subset of {{}} keys
    def replace_simple(s: str, key: str, val: str) -> str:
        return s.replace('{{'+key+'}}', val)

    # joiners
    apis = ', '.join(ctx.get('apis', [])) if ctx.get('apis') else 'None'
    text = text.replace("{{apis | join(\", \") if apis else 'None'}}", apis)

    replacements = {
        'name': ctx.get('name',''),
        'id': ctx.get('id',''),
        'category': ctx.get('category',''),
        'mauEstimate': ctx.get('mauEstimate',''),
        'timeHours': str(ctx.get('timeHours','')),
        'completeness': str(ctx.get('completeness','')),
        'backend': ctx.get('backend','none'),
        'overview': ctx.get('overview',''),
        'targetUsers': ctx.get('targetUsers',''),
        'scenarios': ctx.get('scenarios',''),
        'competitors': ctx.get('competitors',''),
        'ux.pages': ctx.get('ux',{}).get('pages',''),
        'ux.states': ctx.get('ux',{}).get('states',''),
        'ux.cards': ctx.get('ux',{}).get('cards',''),
        'dataModel': ctx.get('dataModel',''),
        'permissions': ', '.join(ctx.get('permissions', [])),
        'capabilityNotes': ctx.get('capabilityNotes',''),
        'apisDesign': ctx.get('apisDesign',''),
        'cacheSync': ctx.get('cacheSync',''),
        'quota': ctx.get('quota',''),
        'analytics.paths': ', '.join(ctx.get('analytics',{}).get('paths', [])),
        'analytics.events': ', '.join(ctx.get('analytics',{}).get('events', [])),
        'backendNotes': ctx.get('backendNotes',''),
        'risks': ctx.get('risks',''),
        'mitigations': ctx.get('mitigations',''),
    }
    for k, v in replacements.items():
        text = replace_simple(text, k, v)

    return text


def load_template() -> str:
    with open(TEMPLATE_PATH, 'r', encoding='utf-8') as f:
        return f.read()


def generate_docs(catalog_path: str, out_dir: str, only: str = None, ids: list[str] | None = None):
    with open(catalog_path, 'r', encoding='utf-8') as f:
        catalog = json.load(f)
    template = load_template()
    os.makedirs(out_dir, exist_ok=True)
    count = 0
    for app in catalog:
        if only and app['id'] != only:
            continue
        if ids and app['id'] not in ids:
            continue
        filename = f"{app['id']}_{app['slug']}.md"
        out_path = os.path.join(out_dir, filename)
        content = render(template, app)
        with open(out_path, 'w', encoding='utf-8') as f:
            f.write(content)
        count += 1
    print(f"Generated {count} app docs into {out_dir} at {datetime.now().isoformat()}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--catalog', required=True)
    parser.add_argument('--out', required=True)
    parser.add_argument('--only')
    parser.add_argument('--ids')
    args = parser.parse_args()
    ids = [s.strip() for s in args.ids.split(',')] if args.ids else None
    generate_docs(args.catalog, args.out, only=args.only, ids=ids)


if __name__ == '__main__':
    main()