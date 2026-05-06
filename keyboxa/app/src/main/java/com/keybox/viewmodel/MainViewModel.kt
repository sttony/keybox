package com.keybox.viewmodel

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import com.keybox.Entry
import com.keybox.Group

class MainViewModel : ViewModel() {
    private val _entries = MutableLiveData<List<Entry>>(emptyList())
    val entries: LiveData<List<Entry>> = _entries

    private val _groups = MutableLiveData<List<Group>>(emptyList())
    val groups: LiveData<List<Group>> = _groups

    private val _isFileLoaded = MutableLiveData<Boolean>(false)
    val isFileLoaded: LiveData<Boolean> = _isFileLoaded

    private val _isUnlocked = MutableLiveData<Boolean>(false)
    val isUnlocked: LiveData<Boolean> = _isUnlocked

    fun setEntries(newEntries: List<Entry>) {
        _entries.value = newEntries
    }

    fun setGroups(newGroups: List<Group>) {
        _groups.value = newGroups
    }

    fun setFileLoaded(loaded: Boolean) {
        _isFileLoaded.value = loaded
    }

    fun setUnlocked(unlocked: Boolean) {
        _isUnlocked.value = unlocked
    }
}
