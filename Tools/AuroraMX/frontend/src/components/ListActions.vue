<script setup lang="ts">
import { Plus, Copy, Trash2, ArrowUp, ArrowDown } from "@lucide/vue";
const p = defineProps<{
  add: () => void;
  clone: () => void;
  remove: () => void;
  up: () => void;
  down: () => void;
  disabled?: boolean;
}>();
</script>
<template>
  <div class="list-actions">
    <button @click="p.add"><Plus :size="12" />Insert</button>
    <button :disabled="p.disabled" @click="p.clone">
      <Copy :size="12" />Clone
    </button>
    <button :disabled="p.disabled" @click="p.remove">
      <Trash2 :size="12" />Delete
    </button>
    <button :disabled="p.disabled" @click="p.up">
      <ArrowUp :size="12" />Up
    </button>
    <button :disabled="p.disabled" @click="p.down">
      <ArrowDown :size="12" />Down
    </button>
  </div>
</template>
