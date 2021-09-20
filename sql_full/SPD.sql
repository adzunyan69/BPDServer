SELECT
/* Запрос по скоростям */
c.UP_NOM    as KOD,
c.NOM       as PUT,
'SPD'       as OBJ,
0           as NUMB,
'Скорости'  as NAME,
d.BEGIN_KM  as B_KM,
d.BEGIN_M   as B_M,
0           as O_KM,
0   	    as O_M,
d.END_KM    as E_KM,
d.END_M     as E_M,
0           as LEN,
d.VSAPS     as VVSK,
d.VLAST     as VSK,
d.VPASS     as VPS,
d.VGR       as VGR,
d.VPOR      as VPR, 
d.VSTRIJ    as VSTR,
d.VALLEGRO  as VALL


FROM WAY c , VUS d

/* WHERE c.assetnum = '110000123030' */
WHERE c.up_nom = :DIR_CODE and c.nom = :TRACK_NUM
/* Параметр  c.assetnum = :P из папспорта проезда Atape */
/* and c.assetnum = :P */
and d.up_nom   = c.up_nom
and d.put_nom  = c.nom


order by
KOD,
PUT,
OBJ,
B_KM,
B_M