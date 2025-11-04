# pyright: reportOptionalMemberAccess=false

import bpy
import mathutils

abs_path = bpy.path.abspath("//")
export_path = f"{abs_path}/../sampleProject/ObjFiles"

def frame_to_time(frame: int):
    return (frame - bpy.context.scene.frame_start) / bpy.context.scene.render.fps

def export_mesh(obj):
    for ob in bpy.data.objects:
        ob.select_set(False)
    obj.select_set(True)
    backup_matrix = obj.matrix_world.copy()
    obj.matrix_world = mathutils.Matrix.Identity(4)
    bpy.ops.wm.obj_export(
        filepath=f"{export_path}/{obj.name}.obj",
        apply_modifiers=True,
        export_selected_objects=True,
        export_materials=False
    )
    y_up_rot = mathutils.Matrix((
        (1, 0, 0, 0),
        (0, 0, 1, 0),
        (0, -1, 0, 0),
        (0, 0, 0, 1)
    ))
    y_up_matrix = y_up_rot @ backup_matrix @ y_up_rot.inverted()
    with open(f"{export_path}/{obj.name}.obj", "a") as f:
        f.write("\nTRANSFORM")
        for row in y_up_matrix:
            for val in row:
                f.write(f" {val}")
        f.write("\n")
    obj.matrix_world = backup_matrix
    obj.select_set(False)

def export_animation(obj):
    output = ""
    if obj.animation_data and obj.animation_data.action:
        frames = {}
        for fcurve in obj.animation_data.action.fcurves:
            for keyframe in fcurve.keyframe_points:
                #frames.append((int(keyframe.co.x)))
                if int(keyframe.co.x) not in frames:
                    frames[int(keyframe.co.x)] = [keyframe.interpolation]
                else:
                    frames[int(keyframe.co.x)].append(keyframe.interpolation)
        scene = bpy.context.scene
        depsgraph = bpy.context.evaluated_depsgraph_get()
        for fr in sorted(frames.keys()):
            scene.frame_set(fr)
            eval_obj = obj.evaluated_get(depsgraph)
            loc, rot, scale = eval_obj.matrix_world.decompose()
            t = frame_to_time(fr)
            most_frequent_interp = max(set(frames[fr]), key=frames[fr].count)
            output += f"{t},{most_frequent_interp},{loc.x},{loc.y},{loc.z},{rot.x},{rot.y},{rot.z},{rot.w},{scale.x},{scale.y},{scale.z}\n"
        if output != "":
            with open(f"{export_path}/{obj.name}.animation", "w") as f:
                f.write(output)

if bpy.context.scene is not None:
    for obj in bpy.data.objects:
        export_mesh(obj)
        export_animation(obj)