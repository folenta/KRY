#!/usr/bin/env python3

"""
    KRY - Kryptografia na eliptickych krivkach
    Jan Folenta (xfolen00)
    6.5.2021
"""

import sys

# Parametre eliptickej krivky zo zadania
FP = 0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff
A = -0x3
B = 0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b
BASE_POINT = (0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296,
              0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5)


""" Funckia getPublicKey ziska zo zadaneho vstupu suradnice verejneho kluca """
def getPublicKey(inputKey):
    publicKeyX = inputKey.split(',')[0][1:]
    publicKeyY = inputKey.split(',')[1][:-1]

    publicKeyXInt = int(publicKeyX, 0)
    publicKeyYInt = int(publicKeyY, 0)

    return publicKeyXInt, publicKeyYInt


""" Funckia checkGivenPoints skontroluje, ci sa zadane body nachadzaju na eliptickej krivke """
def checkGivenPoints(basePoint, publicKey):
    (Xb, Yb) = basePoint
    (Xp, Yp) = publicKey

    if pow(Yb, 2) % FP != (pow(Xb, 3) + A * Xb + B) % FP:
        print("Given base point is not on ECC!")
        exit(1)

    if pow(Yp, 2) % FP != (pow(Xp, 3) + A * Xp + B) % FP:
        print("Given public key is not on ECC!")
        exit(1)


""" Funkcia getInverse ziska inverznu hodnotu potrebnu pre nasobenie """
def getInverse(value):
    invValue = pow(value, FP - 2, FP)

    return invValue


""" Funkcia find2P scita 2 rovnake body P + P na eliptickej krivke """
def find2P(base_point):
    (Xp, Yp) = base_point

    s = ((3 * pow(Xp, 2) + A) * (getInverse(2 * Yp))) % FP

    Xr = (pow(s, 2) - 2 * Xp) % FP
    Yr = (s * (Xp - Xr) - Yp) % FP

    return Xr, Yr


""" Funckia findXP scita zadane body na eliptickej krivke"""
def findXP(P, Q):
    (Xp, Yp) = P
    (Xq, Yq) = Q

    s = ((Yq - Yp) * getInverse(Xq - Xp)) % FP

    Xr = (pow(s, 2) - Xp - Xq) % FP
    Yr = (s * (Xp - Xr) - Yp) % FP

    return (Xr, Yr)


""" Funkcia decipher najde privatnu kluc k danemu verejnemu klucu"""
def decipher(publicKey):
    # Kontrola, ci sa zadane body nachadzaju na zadanej eliptickej krivke
    checkGivenPoints(BASE_POINT, publicKey)

    i = 1

    if BASE_POINT == publicKey:
        return i

    # Najdenie 2P (teda scitanie dvoch tovnakych bodov P + P)
    XP = find2P(BASE_POINT)
    i += 1

    # K vysledku budeme stale pripocitavat bod az pokym sa vysledok nebude rovnat verejnemu klucu
    while XP != publicKey:
        i += 1
        XP = findXP(BASE_POINT, XP)

    # Privatnym klucom je pocet krokov (pocet scitani bodov), po ktorych najdeme verejny kluc
    return i


if __name__ == '__main__':
    inputPublicKey = sys.argv[1]

    publicKey = getPublicKey(inputPublicKey)
    print(decipher(publicKey))
