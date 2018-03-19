from common import *

def set_ssnet_particle_reco_id(df):
    df['reco_e_id'] = df.apply(lambda x : np.argmax(x['p08_shr_shower_frac']),axis=1)
    df['reco_p_id'] = df.apply(lambda x : np.argmin(x['p08_shr_shower_frac']),axis=1)
    return df
    

def xing(row):
    default=-0.000001
    
    if type(row['pt_xing_vv']) == float: return default
    
    xing_stack = np.vstack(row['pt_xing_vv'])
    conn_stack = np.vstack(row['connected_vv'])
    
    rows, cols = np.where(xing_stack==2)
    
    nrows = rows.size
    
    if nrows==0: return default
 
    nconn = float(conn_stack[rows,cols].sum())
    nrows = float(nrows)
    
    nc = nconn / nrows
    
    return nc

def xing_plane(row,plane):
    default=0
    
    if type(row['pt_xing_vv']) == float: return default
    if type(row['connected_vv']) == float: return default
    
    xing_stack = np.vstack(row['pt_xing_vv'])[plane,:]
    conn_stack = np.vstack(row['connected_vv'])[plane,:]
    
    id_v = np.where(xing_stack==2)[0]

    if id_v.size==0: return default
 
    nc = float(conn_stack[id_v].sum())

    return nc / float(id_v.size)


def in_fiducial(row):
    
    X = float(row['anatrk1_vtx_x'])
    Y = float(row['anatrk1_vtx_y'])
    Z = float(row['anatrk1_vtx_z'])
    
    XX = 10.0
    YY = 20.0
    ZZ = 10.0
    
    if (Z<ZZ        or Z>(1036-ZZ)): return 0
    if (X<XX        or X>(256-XX)):  return 0
    if (Y<(-111+YY) or Y>(111-YY)):  return 0

    return 1

rrange = np.arange(5,20).astype(np.float32)

def cos(row):
    plane=2
    
    n2_idx = np.where(row['pt_xing_vv'][plane]==2)[0]
    if n2_idx.size==0: return -1.1

    distance_v = row['distance_vv'][plane][n2_idx]
    radius_v = rrange[n2_idx]
    
    cos  = 2*np.power(radius_v,2.0) - np.power(distance_v,2.0)
    cos /= (2*radius_v*radius_v)
    
    return list(cos)

def three_planes_two(row):
    stack = np.vstack(row['pt_xing_vv'])

    res = np.ones(15)*0
    
    slice_ = ((stack[0,:]==2)&(stack[1,:]==2)&(stack[2,:]==2))
    
    s_v = np.where(slice_)[0]
    if s_v.size==0: return list(res)
    
    res[s_v] = 1
    return list(res)
    
def two_planes_two(row):
    stack = np.vstack(row['pt_xing_vv'])

    res = np.ones(15)*0

    slice_  = ((stack[0,:]==2)&(stack[1,:]==2))
    slice_ |= ((stack[0,:]==2)&(stack[2,:]==2))
    slice_ |= ((stack[1,:]==2)&(stack[2,:]==2))

    s_v = np.where(slice_)[0]
    if s_v.size==0: return list(res)
    
    res[s_v]=1
    return list(res)

def cos2(row,plane):

    res = np.ones(15)*-1.0
    n2_idx = np.where(row['pt_xing_vv'][plane]==2)[0]
    if n2_idx.size==0: return list(res)

    distance_v = row['distance_vv'][plane][n2_idx]
    radius_v = rrange[n2_idx]
    
    cos  = 2*np.power(radius_v,2.0) - np.power(distance_v,2.0)
    cos /= (2*radius_v*radius_v)
    
    res[n2_idx] = np.arccos(cos)*180.0/np.pi
    
    return list(res)



def get_1e1p_hypope(row):
    tid = int(row['TEST_trackid'])
    eid = int(row['reco_e_id'])

    ret_v = [-1.0]*32
    
    stack = np.vstack(row['proton_shower_pair_vv'])
    
    slice_ = ((stack[:,0]==tid) & (stack[:,1]==eid))
    idx_v = np.where(slice_)[0]
    
    if idx_v.size==0: return ret_v
    
    idx = idx_v[0]
    
    ret_v = list(row['proton_shower_hypo_pe_vv'][idx])
    
    return ret_v

def get_1e1p_datape(row):
    tid = int(row['TEST_trackid'])
    eid = int(row['reco_e_id'])

    ret_v = [-1.0]*32
    
    stack = np.vstack(row['proton_shower_pair_vv'])
    
    slice_ = ((stack[:,0]==tid) & (stack[:,1]==eid))
    idx_v = np.where(slice_)[0]
    
    if idx_v.size==0: return ret_v
    
    idx = idx_v[0]
    
    flash_id = int(row['proton_shower_best_data_flash_v'][idx])
    
    ret_v = list(row['data_pe_vv'][flash_id])
    
    return ret_v


def prepare_precuts(df):
    df['fiducial'] = df.apply(in_fiducial,axis=1)
    df['two_pt']   = df.apply(xing,axis=1)
    df['cos']      = df.apply(cos,axis=1)
    df['cos2_p0']  = df.apply(cos2,args=(0,),axis=1)
    df['cos2_p1']  = df.apply(cos2,args=(1,),axis=1)
    df['cos2_p2']  = df.apply(cos2,args=(2,),axis=1)
    df['three_planes_two'] = df.apply(three_planes_two,axis=1)
    df['two_planes_two']   = df.apply(two_planes_two,axis=1)
    df['two_pt_p0']   = df.apply(xing_plane,args=(0,),axis=1)
    df['two_pt_p1']   = df.apply(xing_plane,args=(1,),axis=1)
    df['two_pt_p2']   = df.apply(xing_plane,args=(2,),axis=1)
    return df


def reco_proton_track_id(row):
    pgtrk_v   = row['pgtrk_trk_type_v']
    pgtrk_vv  = row['pgtrk_trk_type_vv']
    protonid  = row['reco_p_id']
    trkid_v = np.where(pgtrk_v==protonid)[0]

    if trkid_v.size == 0: 
        return int(-1)
    
    elif trkid_v.size == 1 : 
        if pgtrk_vv[trkid_v[0]][protonid] == 0: 
            return int(-1)
        else:
            return int(trkid_v[0])
    else:
        stacked   = np.vstack(pgtrk_vv[trkid_v])[:,protonid]
        maxid_trk = np.argmax(stacked)
        if stacked[maxid_trk]==0: 
            return int(-1)
        else: 
            return int(maxid_trk)
        
def reco_proton_track_param(row,param):
    res = -1.0
    if row['reco_proton_trackid']<0: return res
    return row[param][row['reco_proton_trackid']]
    
