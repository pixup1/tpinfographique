# pyright: reportOptionalMemberAccess=false

import bpy
from bpy_extras import anim_utils
import mathutils
import math

abs_path = bpy.path.abspath("//")
project_path = f"{abs_path}/../sampleProject/"
obj_path = f"{project_path}/ObjFiles/"
anim_path = f"{project_path}/Animation/"

def frame_to_time(frame: int):
    return (frame - bpy.context.scene.frame_start) / bpy.context.scene.render.fps

def export_mesh(obj):
    for ob in bpy.data.objects:
        ob.select_set(False)
    obj.select_set(True)
    with open(f"{obj_path}/{obj.name}.obj", "w"):
        pass
    bpy.ops.wm.obj_export(
        filepath=f"{obj_path}/{obj.name}.obj",
        apply_modifiers=True,
        export_selected_objects=True,
        export_materials=False,
        apply_transform=False
    )
    y_up_rot = mathutils.Matrix((
        (1, 0, 0, 0),
        (0, 0, 1, 0),
        (0, -1, 0, 0),
        (0, 0, 0, 1)
    ))
    y_up_matrix = y_up_rot @ obj.matrix_world @ y_up_rot.inverted()
    with open(f"{obj_path}/{obj.name}.obj", "a") as f:
        f.write("\nTRANSFORM")
        for row in y_up_matrix:
            for val in row:
                f.write(f" {val}")
        f.write("\n")
    obj.select_set(False)

def export_animation(obj):
    output = ""
    anim_data = obj.animation_data
    if anim_data is not None and anim_data.action is not None:
        action = anim_data.action
        slot = anim_data.action_slot
        if slot is None:
            slot = anim_data.action_suitable_slots[0]
            anim_data.action_slot = slot
        channelbag = anim_utils.action_get_channelbag_for_slot(action, slot)
        frames = {}
        for fcurve in channelbag.fcurves: # pyright: ignore[reportAttributeAccessIssue]
            for keyframe in fcurve.keyframe_points:
                frame = int(keyframe.co.x)
                interp = keyframe.interpolation
                if frame not in frames:
                    frames[frame] = [interp]
                else:
                    frames[frame].append(interp)
        scene = bpy.context.scene
        depsgraph = bpy.context.evaluated_depsgraph_get()
        prev_rot = None
        for fr in sorted(frames.keys()):
            scene.frame_set(fr)
            eval_obj = obj.evaluated_get(depsgraph)
            loc, rot, scale = eval_obj.matrix_world.decompose()
            if obj.type == 'CAMERA': # Cameras aren't the same in sfml and Blender
                rot = rot @ mathutils.Euler((math.radians(-90), 0, 0), 'XYZ').to_quaternion()
            # Ensure quaternion continuity to avoid flipping
            if prev_rot is not None and rot.dot(prev_rot) < 0: # ngl copilot did this so idk (it avoids the camera flipping)
                rot = -rot
            prev_rot = rot.copy()
            t = frame_to_time(fr)
            most_frequent_interp = max(set(frames[fr]), key=frames[fr].count)
            output += f"{t},{most_frequent_interp},{loc.x},{loc.y},{loc.z},{rot.x},{rot.y},{rot.z},{rot.w},{scale.x},{scale.y},{scale.z}\n"
        if output != "":
            with open(f"{anim_path}/{obj.name}.animation", "w") as f:
                f.write(output)

col = bpy.data.collections["EXPORT"]
if col is not None:
    for obj in col.objects:
        if obj.type != 'CAMERA':
            export_mesh(obj)
        export_animation(obj)
else:
    print("No 'EXPORT' collection found.")